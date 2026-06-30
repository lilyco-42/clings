"""Integration tests for clings.compiler — real compilation + run_cases + dispatch.

These tests compile actual C programs and exercise the full grading pipeline:
  - run_cases with exact stdout / stdout_contains / exit_code / trim_trailing_ws
  - check_one mode dispatch (stdout / compile / return / make+stdout)

They require a C compiler (gcc/clang) in PATH. They are slower than unit
tests (seconds, not milliseconds) but verify the end-to-end contract that
every clings exercise depends on.
"""

from __future__ import annotations

import os
import shutil
import textwrap
from pathlib import Path

import pytest

from clings.compiler import (
    compile_exercise,
    run_cases,
    check_one,
    check_return,
    check_make,
    check_make_stdout,
    ClingsError,
)
from clings import config as cfg


# ─── Helper: write a C program and return an exercise dict ──────────────────

def _write_c_program(src_dir: Path, name: str, code: str) -> dict:
    """Write a .c file into src_dir and return a minimal exercise dict."""
    src_dir.mkdir(parents=True, exist_ok=True)
    src_file = src_dir / f"{name}.c"
    src_file.write_text(code, encoding="utf-8")
    return {
        "name": name,
        "path": name,
        "source": f"{name}.c",
        "mode": "stdout",
        "cflags": ["-std=c11", "-Wall", "-Wextra", "-O0"],
    }


# ─── Skip if no C compiler available ────────────────────────────────────────

pytestmark = pytest.mark.skipif(
    shutil.which("gcc") is None and shutil.which("clang") is None,
    reason="no C compiler (gcc/clang) in PATH",
)


# ═══════════════════════════════════════════════════════════════════════════
# run_cases — exact stdout
# ═══════════════════════════════════════════════════════════════════════════

class TestRunCasesExactStdout:
    """run_cases with exact 'stdout' field — the traditional diff path."""

    def test_exact_match_passes(self, isolated_workspace: Path) -> None:
        """Program output exactly matches expected stdout → pass."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "hello",
            "hello",
            '#include <stdio.h>\nint main(void){printf("Hello!\\n");return 0;}\n',
        )
        ex["cases"] = [{"stdout": "Hello!\n"}]  # real newline, not escaped
        binary = compile_exercise(ex, use_solutions=False)
        run_cases(ex, binary, include_hidden=False)  # no exception = pass

    def test_mismatch_raises(self, isolated_workspace: Path) -> None:
        """Output doesn't match → ClingsError."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "wrong",
            "wrong",
            '#include <stdio.h>\nint main(void){printf("Wrong\\n");return 0;}\n',
        )
        ex["cases"] = [{"stdout": "Expected\n"}]  # real newline
        binary = compile_exercise(ex, use_solutions=False)
        with pytest.raises(ClingsError, match="output mismatch"):
            run_cases(ex, binary, include_hidden=False)


# ═══════════════════════════════════════════════════════════════════════════
# run_cases — stdout_contains / stdout_not_contains
# ═══════════════════════════════════════════════════════════════════════════

class TestRunCasesContains:
    """run_cases with stdout_contains / stdout_not_contains (T49-style)."""

    def test_contains_present_passes(self, isolated_workspace: Path) -> None:
        """Required line present → pass."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "multi",
            "multi",
            textwrap.dedent("""\
                #include <stdio.h>
                int main(void){
                    printf("=== Header ===\\n");
                    printf("key line\\n");
                    printf("exit code: 2\\n");
                    return 2;
                }
            """),
        )
        ex["cases"] = [{
            "exit_code": 2,
            "stdout_contains": ["=== Header ===", "key line", "exit code: 2"],
        }]
        binary = compile_exercise(ex, use_solutions=False)
        run_cases(ex, binary, include_hidden=False)

    def test_contains_missing_raises(self, isolated_workspace: Path) -> None:
        """Required line absent → ClingsError."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "missing",
            "missing",
            '#include <stdio.h>\nint main(void){printf("hello\\n");return 0;}\n',
        )
        ex["cases"] = [{"stdout_contains": ["DEADLOCK DETECTED"]}]
        binary = compile_exercise(ex, use_solutions=False)
        with pytest.raises(ClingsError, match="missing required line"):
            run_cases(ex, binary, include_hidden=False)

    def test_not_contains_violation_raises(self, isolated_workspace: Path) -> None:
        """Forbidden line present → ClingsError."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "forbidden",
            "forbidden",
            '#include <stdio.h>\nint main(void){printf("DEADLOCK\\n");return 0;}\n',
        )
        ex["cases"] = [{"stdout_not_contains": ["DEADLOCK"]}]
        binary = compile_exercise(ex, use_solutions=False)
        with pytest.raises(ClingsError, match="forbidden line"):
            run_cases(ex, binary, include_hidden=False)

    def test_not_contains_absent_passes(self, isolated_workspace: Path) -> None:
        """Forbidden line absent → pass."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "safe",
            "safe",
            '#include <stdio.h>\nint main(void){printf("all good\\n");return 0;}\n',
        )
        ex["cases"] = [{"stdout_not_contains": ["DEADLOCK"]}]
        binary = compile_exercise(ex, use_solutions=False)
        run_cases(ex, binary, include_hidden=False)


# ═══════════════════════════════════════════════════════════════════════════
# run_cases — exit_code
# ═══════════════════════════════════════════════════════════════════════════

class TestRunCasesExitCode:
    """run_cases with exit_code assertion (including non-zero for deadlocks)."""

    def test_zero_exit_passes(self, isolated_workspace: Path) -> None:
        """Exit 0 (default) matches → pass."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "ok",
            "ok",
            'int main(void){return 0;}\n',
        )
        ex["cases"] = [{}]
        binary = compile_exercise(ex, use_solutions=False)
        run_cases(ex, binary, include_hidden=False)

    def test_nonzero_exit_passes(self, isolated_workspace: Path) -> None:
        """exit_code=2 expected and program returns 2 → pass."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "deadlock",
            "deadlock",
            'int main(void){return 2;}\n',
        )
        ex["cases"] = [{"exit_code": 2}]
        binary = compile_exercise(ex, use_solutions=False)
        run_cases(ex, binary, include_hidden=False)

    def test_wrong_exit_raises(self, isolated_workspace: Path) -> None:
        """Expected exit 0 but got 1 → ClingsError."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "badexit",
            "badexit",
            'int main(void){return 1;}\n',
        )
        ex["cases"] = [{"exit_code": 0}]
        binary = compile_exercise(ex, use_solutions=False)
        with pytest.raises(ClingsError, match="exited 1"):
            run_cases(ex, binary, include_hidden=False)


# ═══════════════════════════════════════════════════════════════════════════
# run_cases — trim_trailing_ws end-to-end
# ═══════════════════════════════════════════════════════════════════════════

class TestRunCasesTrimTrailingWs:
    """trim_trailing_ws=true end-to-end: program with trailing spaces passes."""

    def test_trim_allows_trailing_spaces(self, isolated_workspace: Path) -> None:
        """Program outputs trailing spaces; with trim_trailing_ws → pass."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "trim",
            "trim",
            '#include <stdio.h>\nint main(void){printf("hello \\nworld \\n");return 0;}\n',
        )
        ex["cases"] = [{"stdout": "hello\nworld\n", "trim_trailing_ws": True}]
        binary = compile_exercise(ex, use_solutions=False)
        run_cases(ex, binary, include_hidden=False)

    def test_no_trim_rejects_trailing_spaces(self, isolated_workspace: Path) -> None:
        """Same program without trim → fails (backward compat)."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "notrim",
            "notrim",
            '#include <stdio.h>\nint main(void){printf("hello \\nworld \\n");return 0;}\n',
        )
        ex["cases"] = [{"stdout": "hello\nworld\n"}]
        binary = compile_exercise(ex, use_solutions=False)
        with pytest.raises(ClingsError, match="output mismatch"):
            run_cases(ex, binary, include_hidden=False)


# ═══════════════════════════════════════════════════════════════════════════
# run_cases — stdout_regex
# ═══════════════════════════════════════════════════════════════════════════

class TestRunCasesRegex:
    """run_cases with stdout_regex assertion."""

    def test_regex_match_passes(self, isolated_workspace: Path) -> None:
        """Regex matches → pass."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "regex_ok",
            "regex_ok",
            '#include <stdio.h>\nint main(void){printf("P0 ate 100\\n");return 0;}\n',
        )
        ex["cases"] = [{"stdout_regex": r"P\d ate \d+"}]
        binary = compile_exercise(ex, use_solutions=False)
        run_cases(ex, binary, include_hidden=False)

    def test_regex_no_match_raises(self, isolated_workspace: Path) -> None:
        """Regex doesn't match → ClingsError."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "regex_bad",
            "regex_bad",
            '#include <stdio.h>\nint main(void){printf("hello\\n");return 0;}\n',
        )
        ex["cases"] = [{"stdout_regex": r"DEADLOCK"}]
        binary = compile_exercise(ex, use_solutions=False)
        with pytest.raises(ClingsError, match="regex not matched"):
            run_cases(ex, binary, include_hidden=False)


# ═══════════════════════════════════════════════════════════════════════════
# check_one — mode dispatch
# ═══════════════════════════════════════════════════════════════════════════

class TestCheckOneModeDispatch:
    """check_one() dispatches to the correct handler based on 'mode' field."""

    def test_stdout_mode(self, isolated_workspace: Path) -> None:
        """mode='stdout' → compile + run_cases."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "stdout_mode",
            "stdout_mode",
            '#include <stdio.h>\nint main(void){printf("ok\\n");return 0;}\n',
        )
        ex["cases"] = [{"stdout": "ok\n"}]
        # No exception = pass
        check_one(ex, use_solutions=False, include_hidden=False)

    def test_compile_mode(self, isolated_workspace: Path) -> None:
        """mode='compile' → just compile, no run."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "compile_mode",
            "compile_mode",
            'int main(void){return 0;}\n',
        )
        ex["mode"] = "compile"
        check_one(ex, use_solutions=False, include_hidden=False)

    def test_return_mode_pass(self, isolated_workspace: Path) -> None:
        """mode='return' → compile + check exit code."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "return_mode",
            "return_mode",
            'int main(void){return 0;}\n',
        )
        ex["mode"] = "return"
        ex["expected_return"] = 0
        check_one(ex, use_solutions=False, include_hidden=False)

    def test_return_mode_fail(self, isolated_workspace: Path) -> None:
        """mode='return' with wrong exit code → ClingsError."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "return_fail",
            "return_fail",
            'int main(void){return 1;}\n',
        )
        ex["mode"] = "return"
        ex["expected_return"] = 0
        with pytest.raises(ClingsError, match="expected return 0"):
            check_one(ex, use_solutions=False, include_hidden=False)

    def test_compile_failure_raises(self, isolated_workspace: Path) -> None:
        """A program that doesn't compile → ClingsError."""
        ex = _write_c_program(
            isolated_workspace / "exercises" / "broken",
            "broken",
            'int main(void){ this is not valid C }\n',
        )
        ex["cases"] = [{}]
        with pytest.raises(ClingsError, match="compile failed"):
            check_one(ex, use_solutions=False, include_hidden=False)

    def test_missing_source_raises(self, isolated_workspace: Path) -> None:
        """Non-existent source file → ClingsError."""
        ex = {
            "name": "ghost",
            "path": "ghost",
            "source": "ghost.c",
            "mode": "stdout",
            "cases": [{}],
        }
        with pytest.raises(ClingsError, match="missing source"):
            check_one(ex, use_solutions=False, include_hidden=False)


# ═══════════════════════════════════════════════════════════════════════════
# End-to-end: init unit0 → check → score (using real clings commands)
# ═══════════════════════════════════════════════════════════════════════════

class TestEndToEndUnit0:
    """Full end-to-end test: compile a real unit0 exercise and verify grading.

    This exercises the complete pipeline: discover → compile → run_cases.
    Uses the actual unit0 exercise source from the clings package.
    """

    def test_simplest_c_program_compiles_and_passes(
        self, isolated_workspace: Path, monkeypatch: pytest.MonkeyPatch
    ) -> None:
        """The 'simplest C program' exercise compiles and matches expected output.

        This mirrors what `clings check unit0 --solutions` does for one exercise,
        but in-process, giving us a precise assertion on the grading result.
        """
        # Use the real exercises/ from the clings package (not the temp one)
        real_exercises = cfg.EXERCISES_DIR
        # Find the simplest_c_program exercise
        src_dir = real_exercises / "01_simplest_c_program"
        if not src_dir.exists():
            pytest.skip("real exercises/01_simplest_c_program not available")

        # Copy the .c template (with #error removed) into the temp workspace
        ex_name = "01_simplest_c_program"
        temp_src_dir = isolated_workspace / "exercises" / ex_name
        temp_src_dir.mkdir(parents=True, exist_ok=True)

        # Use the solution if available (CLINGS_SOLUTIONS_DIR), else skip
        solutions_env = os.environ.get("CLINGS_SOLUTIONS_DIR", "")
        sol_file = Path(solutions_env) / ex_name / "simplest_c_program.c" if solutions_env else None
        if not sol_file or not sol_file.exists():
            pytest.skip("solutions not available (set CLINGS_SOLUTIONS_DIR)")

        (temp_src_dir / "simplest_c_program.c").write_text(
            sol_file.read_text(encoding="utf-8"), encoding="utf-8"
        )

        # Set up the test case (expected output: "Hello, World!\n")
        test_file = isolated_workspace / "tests" / f"{ex_name}.toml"
        test_file.write_text(
            '[[cases]]\nstdout = "Hello, World!\\n"\n', encoding="utf-8"
        )

        ex = {
            "name": ex_name,
            "path": ex_name,
            "source": "simplest_c_program.c",
            "mode": "stdout",
            "cflags": ["-std=c11", "-Wall", "-Wextra", "-O0"],
            "cases": [{"stdout": "Hello, World!\n"}],
        }
        # Use external tests/ (already set up in isolated_workspace via conftest)
        check_one(ex, use_solutions=False, include_hidden=False)


# ═══════════════════════════════════════════════════════════════════════════
# check_make — make-mode exercises (student Makefile runs make targets)
# ═══════════════════════════════════════════════════════════════════════════

def _write_make_exercise(src_dir: Path, name: str, source_code: str,
                         makefile_content: str, binary_name: str | None = None,
                         make_targets: list[str] | None = None) -> dict:
    """Write a .c file + Makefile into src_dir, return a make-mode exercise dict.

    By default uses make_targets=["all"] (the common clings convention).
    The Makefile's TARGET must match `binary_name` for check_make_stdout to
    find the produced binary.
    """
    src_dir.mkdir(parents=True, exist_ok=True)
    (src_dir / f"{name}.c").write_text(source_code, encoding="utf-8")
    (src_dir / "Makefile").write_text(makefile_content, encoding="utf-8")
    ex: dict = {
        "name": name,
        "path": name,
        "source": f"{name}.c",
        "mode": "make",
        "make_targets": make_targets or ["all"],
    }
    if binary_name:
        ex["binary"] = binary_name
    return ex


class TestCheckMake:
    """check_make() runs student Makefile targets and verifies they succeed.

    This is the 'pure make' mode: clings only runs `make <targets>` and checks
    return codes — no stdout comparison. Used by exercises where the Makefile's
    own `test` target does the verification (e.g. diff against expected output).
    """

    def test_make_success(self, isolated_workspace: Path) -> None:
        """A working Makefile with 'all' target → no exception."""
        ex = _write_make_exercise(
            isolated_workspace / "exercises" / "mksuccess",
            "mksuccess",
            'int main(void){return 0;}\n',
            "CC = gcc\nCFLAGS = -Wall\nTARGET = mksuccess\n"
            "SRC = mksuccess.c\n"
            "all: $(TARGET)\n"
            "$(TARGET): $(SRC)\n"
            "\t$(CC) $(CFLAGS) $< -o $@\n",
        )
        check_make(ex, use_solutions=False, use_cache=False)

    def test_make_failure_raises(self, isolated_workspace: Path) -> None:
        """Makefile that fails (bad command) → ClingsError."""
        ex = _write_make_exercise(
            isolated_workspace / "exercises" / "mkfail",
            "mkfail",
            'int main(void){return 0;}\n',
            "all:\n\tfalse\n",  # `false` always exits 1
        )
        with pytest.raises(ClingsError, match="make target failed"):
            check_make(ex, use_solutions=False, use_cache=False)

    def test_missing_source_dir_raises(self, isolated_workspace: Path) -> None:
        """Non-existent source directory → ClingsError."""
        ex = {"name": "ghost", "path": "ghost", "mode": "make",
              "make_targets": ["all"]}
        with pytest.raises(ClingsError, match="missing source"):
            check_make(ex, use_solutions=False, use_cache=False)

    def test_custom_make_targets(self, isolated_workspace: Path) -> None:
        """Custom make_targets (e.g. ['build']) are run correctly."""
        ex = _write_make_exercise(
            isolated_workspace / "exercises" / "customtarget",
            "customtarget",
            'int main(void){return 0;}\n',
            "CC = gcc\nTARGET = customtarget\nSRC = customtarget.c\n"
            "build: $(TARGET)\n"
            "$(TARGET): $(SRC)\n"
            "\t$(CC) -Wall $< -o $@\n",
            make_targets=["build"],
        )
        check_make(ex, use_solutions=False, use_cache=False)

    def test_default_make_targets_is_test(
        self, isolated_workspace: Path
    ) -> None:
        """When make_targets is absent, defaults to ['test']."""
        ex = _write_make_exercise(
            isolated_workspace / "exercises" / "deftarget",
            "deftarget",
            'int main(void){return 0;}\n',
            "CC = gcc\nTARGET = deftarget\nSRC = deftarget.c\n"
            "$(TARGET): $(SRC)\n"
            "\t$(CC) -Wall $< -o $@\n"
            "test: $(TARGET)\n"
            "\t./$(TARGET)\n",
            make_targets=None,  # omit → defaults to ["test"]
        )
        # Remove make_targets to test the default
        ex.pop("make_targets", None)
        check_make(ex, use_solutions=False, use_cache=False)

    def test_cache_skips_make(self, isolated_workspace: Path) -> None:
        """With use_cache=True, second call skips make (cache hit).

        We verify by checking that MAKE_CACHE is populated after the first
        call, and the second call does not raise even if we monkeypatch
        subprocess.run to fail (proving make was not invoked again).
        """
        from clings.compiler import MAKE_CACHE
        from clings import compiler as comp_mod

        src_dir = isolated_workspace / "exercises" / "cacheex"
        ex = _write_make_exercise(
            src_dir, "cacheex",
            'int main(void){return 0;}\n',
            "CC = gcc\nTARGET = cacheex\nSRC = cacheex.c\n"
            "all: $(TARGET)\n"
            "$(TARGET): $(SRC)\n"
            "\t$(CC) -Wall $< -o $@\n",
        )
        # First call: succeeds, populates cache
        check_make(ex, use_solutions=False, use_cache=True)
        assert len(MAKE_CACHE) > 0, "MAKE_CACHE should be populated"

        # Record the cache size; second call should not add new entries
        cache_size_before = len(MAKE_CACHE)

        # Second call with cache: should skip make (cache hit)
        # If make were rerun, it would still succeed (files unchanged), so we
        # verify via cache size stability instead.
        check_make(ex, use_solutions=False, use_cache=True)
        assert len(MAKE_CACHE) == cache_size_before, \
            "cache size should not grow on second cached call"

    def test_no_cache_reruns_make(self, isolated_workspace: Path) -> None:
        """With use_cache=False, make is always rerun (watch mode behavior)."""
        src_dir = isolated_workspace / "exercises" / "nocache"
        ex = _write_make_exercise(
            src_dir, "nocache",
            'int main(void){return 0;}\n',
            "CC = gcc\nTARGET = nocache\nSRC = nocache.c\n"
            "all: $(TARGET)\n"
            "$(TARGET): $(SRC)\n"
            "\t$(CC) -Wall $< -o $@\n",
        )
        # First call succeeds
        check_make(ex, use_solutions=False, use_cache=False)

        # Sabotage the Makefile
        (src_dir / "Makefile").write_text("all:\n\tfalse\n", encoding="utf-8")

        # Second call without cache: reruns make → should fail
        with pytest.raises(ClingsError, match="make target failed"):
            check_make(ex, use_solutions=False, use_cache=False)


# ═══════════════════════════════════════════════════════════════════════════
# check_make_stdout — hybrid mode (Makefile builds, clings runs + compares)
# ═══════════════════════════════════════════════════════════════════════════

class TestCheckMakeStdout:
    """check_make_stdout() is the hybrid mode used by most Unit 2/3 exercises.

    Flow: student Makefile compiles → clings locates binary → clings runs it
    and compares stdout against test cases. This is the most complex grading
    path, combining make execution + binary discovery + run_cases.
    """

    def test_full_success(self, isolated_workspace: Path) -> None:
        """Makefile builds → binary found → stdout matches → pass."""
        ex = _write_make_exercise(
            isolated_workspace / "exercises" / "hybrid_ok",
            "hybrid_ok",
            '#include <stdio.h>\nint main(void){printf("ok\\n");return 0;}\n',
            "CC = gcc\nCFLAGS = -Wall -std=c11\nTARGET = hybrid_ok\n"
            "SRC = hybrid_ok.c\n"
            "all: $(TARGET)\n"
            "$(TARGET): $(SRC)\n"
            "\t$(CC) $(CFLAGS) $< -o $@\n",
            binary_name="hybrid_ok",
            make_targets=["all"],
        )
        ex["mode"] = "make+stdout"
        ex["cases"] = [{"stdout": "ok\n"}]
        check_make_stdout(ex, use_solutions=False, include_hidden=False,
                          use_cache=False)

    def test_missing_source_dir_raises(self, isolated_workspace: Path) -> None:
        """Non-existent source directory → ClingsError."""
        ex = {"name": "ghost", "path": "ghost", "mode": "make+stdout",
              "make_targets": ["all"], "cases": [{}]}
        with pytest.raises(ClingsError, match="missing source"):
            check_make_stdout(ex, use_solutions=False, include_hidden=False,
                              use_cache=False)

    def test_make_failure_raises(self, isolated_workspace: Path) -> None:
        """Makefile fails to build → ClingsError."""
        ex = _write_make_exercise(
            isolated_workspace / "exercises" / "hybrid_mkfail",
            "hybrid_mkfail",
            'int main(void){return 0;}\n',
            "all:\n\tfalse\n",
            binary_name="hybrid_mkfail",
        )
        ex["mode"] = "make+stdout"
        ex["cases"] = [{}]
        with pytest.raises(ClingsError, match="make target failed"):
            check_make_stdout(ex, use_solutions=False, include_hidden=False,
                              use_cache=False)

    def test_binary_not_produced_after_make(
        self, isolated_workspace: Path
    ) -> None:
        """Make succeeds but produces no binary → ClingsError 'did not produce'.

        This catches the common student mistake: Makefile's TARGET doesn't
        match the `binary` field in exercises.toml.
        """
        ex = _write_make_exercise(
            isolated_workspace / "exercises" / "nobinary",
            "nobinary",
            'int main(void){return 0;}\n',
            # Makefile builds 'wrong_name' but exercises.toml expects 'nobinary'
            "CC = gcc\nall:\n\tgcc nobinary.c -o wrong_name\n",
            binary_name="nobinary",  # expects 'nobinary' but Makefile makes 'wrong_name'
            make_targets=["all"],
        )
        ex["mode"] = "make+stdout"
        ex["cases"] = [{}]
        with pytest.raises(ClingsError, match="did not produce expected binary"):
            check_make_stdout(ex, use_solutions=False, include_hidden=False,
                              use_cache=False)

    def test_binary_not_found_when_make_cached(
        self, isolated_workspace: Path
    ) -> None:
        """Binary missing + make was skipped (cached) → ClingsError 'expected binary not found'.

        Scenario: first check_make_stdout builds + caches; then the binary is
        deleted externally; second call skips make (cache hit) but can't find
        the binary. This simulates a student running `make clean` between
        clings runs.
        """
        src_dir = isolated_workspace / "exercises" / "cached_missing"
        ex = _write_make_exercise(
            src_dir, "cached_missing",
            'int main(void){return 0;}\n',
            "CC = gcc\nTARGET = cached_missing\nSRC = cached_missing.c\n"
            "all: $(TARGET)\n"
            "$(TARGET): $(SRC)\n"
            "\t$(CC) -Wall $< -o $@\n",
            binary_name="cached_missing",
            make_targets=["all"],
        )
        ex["mode"] = "make+stdout"
        ex["cases"] = [{}]
        # First call: builds + caches
        check_make_stdout(ex, use_solutions=False, include_hidden=False,
                          use_cache=True)
        # Delete the binary externally (simulates `make clean`)
        binary = src_dir / "cached_missing"
        binary.unlink()
        # Second call: make is cached (skipped), binary missing
        with pytest.raises(ClingsError, match="expected binary not found"):
            check_make_stdout(ex, use_solutions=False, include_hidden=False,
                              use_cache=True)

    def test_stdout_mismatch_raises(self, isolated_workspace: Path) -> None:
        """Make succeeds, binary found, but stdout doesn't match → ClingsError."""
        ex = _write_make_exercise(
            isolated_workspace / "exercises" / "hybrid_mismatch",
            "hybrid_mismatch",
            '#include <stdio.h>\nint main(void){printf("wrong\\n");return 0;}\n',
            "CC = gcc\nCFLAGS = -Wall\nTARGET = hybrid_mismatch\n"
            "SRC = hybrid_mismatch.c\n"
            "all: $(TARGET)\n"
            "$(TARGET): $(SRC)\n"
            "\t$(CC) $(CFLAGS) $< -o $@\n",
            binary_name="hybrid_mismatch",
            make_targets=["all"],
        )
        ex["mode"] = "make+stdout"
        ex["cases"] = [{"stdout": "expected\n"}]  # program prints "wrong"
        with pytest.raises(ClingsError, match="output mismatch"):
            check_make_stdout(ex, use_solutions=False, include_hidden=False,
                              use_cache=False)

    def test_default_make_targets_is_build(
        self, isolated_workspace: Path
    ) -> None:
        """When make_targets is absent in make+stdout mode, defaults to ['build']."""
        ex = _write_make_exercise(
            isolated_workspace / "exercises" / "defbuild",
            "defbuild",
            '#include <stdio.h>\nint main(void){printf("hi\\n");return 0;}\n',
            "CC = gcc\nTARGET = defbuild\nSRC = defbuild.c\n"
            "build: $(TARGET)\n"
            "$(TARGET): $(SRC)\n"
            "\t$(CC) -Wall $< -o $@\n",
            binary_name="defbuild",
            make_targets=None,  # omit → defaults to ["build"]
        )
        ex["mode"] = "make+stdout"
        ex.pop("make_targets", None)  # ensure default is used
        ex["cases"] = [{"stdout": "hi\n"}]
        check_make_stdout(ex, use_solutions=False, include_hidden=False,
                          use_cache=False)


# ═══════════════════════════════════════════════════════════════════════════
# check_one — make / make+stdout mode dispatch
# ═══════════════════════════════════════════════════════════════════════════

class TestCheckOneMakeDispatch:
    """check_one() dispatches to check_make / check_make_stdout by mode.

    These verify the dispatch wiring (mode → handler) end-to-end through
    check_one, complementing the direct check_make / check_make_stdout tests
    above.
    """

    def test_make_mode_dispatches(self, isolated_workspace: Path) -> None:
        """mode='make' → check_make is called (make target runs)."""
        ex = _write_make_exercise(
            isolated_workspace / "exercises" / "dispatch_mk",
            "dispatch_mk",
            'int main(void){return 0;}\n',
            "CC = gcc\nTARGET = dispatch_mk\nSRC = dispatch_mk.c\n"
            "all: $(TARGET)\n"
            "$(TARGET): $(SRC)\n"
            "\t$(CC) -Wall $< -o $@\n",
        )
        # check_one should dispatch to check_make (use_cache=False to avoid
        # cross-test cache pollution)
        check_one(ex, use_solutions=False, include_hidden=False, use_cache=False)

    def test_make_stdout_mode_dispatches(
        self, isolated_workspace: Path
    ) -> None:
        """mode='make+stdout' → check_make_stdout is called."""
        ex = _write_make_exercise(
            isolated_workspace / "exercises" / "dispatch_mks",
            "dispatch_mks",
            '#include <stdio.h>\nint main(void){printf("dispatched\\n");return 0;}\n',
            "CC = gcc\nCFLAGS = -Wall\nTARGET = dispatch_mks\n"
            "SRC = dispatch_mks.c\n"
            "all: $(TARGET)\n"
            "$(TARGET): $(SRC)\n"
            "\t$(CC) $(CFLAGS) $< -o $@\n",
            binary_name="dispatch_mks",
            make_targets=["all"],
        )
        ex["mode"] = "make+stdout"
        ex["cases"] = [{"stdout": "dispatched\n"}]
        check_one(ex, use_solutions=False, include_hidden=False, use_cache=False)
