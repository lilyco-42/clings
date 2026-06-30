"""Compilation, execution, and test verification logic."""

import os
import re
import subprocess
from pathlib import Path

from .config import (
    BUILD_DIR,
    ROOT,
    ClingsError,
    find_compiler,
    load_toml,
    source_dir_for,
    test_files_for,
)

# Cache for make targets already verified in this session.
# Key: (src_dir, target, use_solutions, mtime_signature)
# mtime_signature = max mtime of build-relevant files (*.c, *.h, Makefile, *.mk)
# in the source directory. When any build input changes, the signature changes,
# invalidating the cache entry so make is re-run.
#
# In watch mode, use_cache=False is passed to bypass this cache entirely: watch
# already detects file changes via mtime polling, and make itself handles
# incremental builds correctly. Caching at the tool layer would only cause
# stale-build bugs (see: Unit 3 make exercises not rebuilding after edits).
MAKE_CACHE: set[tuple[str, str, bool, float]] = set()


def _make_source_mtime_signature(src_dir: Path) -> float:
    """Compute a mtime signature for make-mode build inputs.

    Covers all files that affect a make build: C sources, headers, the
    Makefile itself, and any included .mk fragments. Returns the max mtime
    so that any single file change invalidates the cache.
    """
    max_mtime = 0.0
    for pattern in ("*.c", "*.h", "Makefile", "makefile", "GNUmakefile", "*.mk"):
        for f in src_dir.glob(pattern):
            try:
                max_mtime = max(max_mtime, f.stat().st_mtime)
            except OSError:
                pass
    return max_mtime


def compile_exercise(ex: dict, use_solutions: bool) -> Path:
    """Compile an exercise and return the path to the binary."""
    src_dir = source_dir_for(ex, use_solutions)
    if not src_dir.exists():
        raise ClingsError(f"missing source directory: {src_dir.relative_to(ROOT)}")
    if "sources" in ex:
        c_files = [src_dir / source for source in ex["sources"]]
    elif "source" in ex:
        c_files = [src_dir / ex["source"]]
    else:
        c_files = sorted(src_dir.glob("*.c"))
    missing = [path for path in c_files if not path.exists()]
    if missing:
        names = ", ".join(str(path.relative_to(ROOT)) for path in missing)
        raise ClingsError(f"missing source file(s): {names}")
    if not c_files:
        raise ClingsError(f"no .c files in {src_dir.relative_to(ROOT)}")
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    suffix = ".exe" if os.name == "nt" else ""
    binary = BUILD_DIR / f"{ex['name']}{suffix}"
    compiler = ex.get("compiler") or find_compiler()
    if not compiler:
        raise ClingsError("missing C compiler: install gcc/clang or set CC=/path/to/compiler")
    cflags = ex.get("cflags", ["-std=c11", "-Wall", "-Wextra", "-pedantic", "-O2"])
    cmd = [
        compiler, *[str(flag) for flag in cflags],
        *[str(path) for path in c_files],
        *[str(flag) for flag in ex.get("ldflags", [])],
        "-lm", "-o", str(binary),
    ]
    proc = subprocess.run(cmd, cwd=ROOT, text=True, capture_output=True)
    if proc.returncode != 0:
        raise ClingsError(
            f"compile failed for {ex['name']}\n"
            f"$ {' '.join(cmd)}\n{proc.stderr.strip()}"
        )
    return binary


def normalize(text: str, trim_trailing_ws: bool = False) -> str:
    """Normalize text for output comparison.

    - Always: normalize line endings (``\\r\\n`` → ``\\n``).
    - Optionally: strip trailing whitespace from every line. This relieves
      false-negative failures where the student's algorithm is correct but
      the output carries incidental trailing spaces (a common editor artifact
      and a known pain point in strict stdout-diff grading). The trailing
      newline is NOT touched — POSIX text-file convention (files end with a
      newline) is preserved as a legitimate C-engineering teaching goal.

    Enable per-case via ``trim_trailing_ws = true`` in exercises.toml / tests.
    """
    text = text.replace("\r\n", "\n")
    if trim_trailing_ws:
        text = "\n".join(line.rstrip() for line in text.split("\n"))
    return text


def _collect_cases(ex: dict, include_hidden: bool) -> list[dict]:
    """Collect test cases: prefer external tests/ files, fall back to inline.

    Priority:
      1. External test files (tests/<name>.toml in package or repo root)
      2. Hidden test files (if include_hidden and CLINGS_HIDDEN_TEST_DIR set)
      3. Inline [[exercises.cases]] from exercises.toml (fallback)

    This means exercises.toml may carry a copy of cases for student reference
    (via `clings tests`), but the authoritative source for grading is tests/.
    """
    external_cases = []
    for test_file in test_files_for(ex, include_hidden):
        data = load_toml(test_file)
        external_cases.extend(data.get("cases", []))
    if external_cases:
        return external_cases
    # Fallback: inline cases from exercises.toml
    return list(ex.get("cases", []))


def run_cases(ex: dict, binary: Path, include_hidden: bool) -> None:
    """Run all test cases for an exercise against the compiled binary.

    Per-case assertion fields (all optional, combinable, AND semantics):

      - ``exit_code`` (int, default 0): expected process exit code.
      - ``stdout`` (str): exact stdout match (after line-ending normalization).
      - ``stdout_b64`` (str): base64-encoded exact stdout (for control chars).
      - ``stdout_contains`` (list[str]): every needle must appear as a substring.
      - ``stdout_not_contains`` (list[str]): none of the needles may appear.
      - ``stdout_regex`` (str): a regex that must match somewhere in stdout.
        Anchors are not added; use ``^...$`` if a full-match is desired.
      - ``trim_trailing_ws`` (bool, default false): strip trailing whitespace
        from every line of both expected and actual before comparison. Relieves
        false-negative failures from incidental trailing spaces without relaxing
        the trailing-newline requirement.

    When ``stdout``/``stdout_b64`` is absent, exact-match is skipped — useful
    for non-deterministic outputs (e.g. real multithreaded programs) where
    only structural invariants (presence of key lines, exit code) matter.
    """
    cases = _collect_cases(ex, include_hidden)
    if not cases:
        raise ClingsError(f"no test cases found for {ex['name']}")
    case_no = 0
    for case in cases:
        case_no += 1
        if case.get("compile_only", False):
            continue
        stdin = case.get("stdin", "")
        args = [str(arg) for arg in case.get("args", [])]
        timeout = float(case.get("timeout", 2.0))
        trim_ws = bool(case.get("trim_trailing_ws", False))
        proc = subprocess.run(
            [str(binary), *args],
            input=stdin,
            text=True,
            capture_output=True,
            timeout=timeout,
        )
        actual = normalize(proc.stdout, trim_ws)

        # 1. Exit code check (default 0).
        expected_exit = int(case.get("exit_code", 0))
        if proc.returncode != expected_exit:
            raise ClingsError(
                f"{ex['name']} case {case_no} exited {proc.returncode} "
                f"(expected {expected_exit})\n"
                f"stderr:\n{proc.stderr.strip()}"
            )

        # 2. Exact stdout match (only when the field is present).
        if "stdout_b64" in case:
            import base64
            expected = base64.b64decode(case["stdout_b64"]).decode("utf-8", errors="replace")
            if actual != normalize(expected, trim_ws):
                raise ClingsError(
                    f"{ex['name']} case {case_no} output mismatch\n"
                    f"stdin:\n{stdin}"
                    f"expected:\n{expected}"
                    f"actual:\n{actual}"
                )
        elif "stdout" in case:
            expected = case["stdout"]
            if actual != normalize(expected, trim_ws):
                raise ClingsError(
                    f"{ex['name']} case {case_no} output mismatch\n"
                    f"stdin:\n{stdin}"
                    f"expected:\n{expected}"
                    f"actual:\n{actual}"
                )

        # 3. Substring presence checks (non-deterministic-friendly).
        for needle in case.get("stdout_contains", []):
            if needle not in actual:
                raise ClingsError(
                    f"{ex['name']} case {case_no} missing required line:\n"
                    f"  expected substring: {needle!r}\n"
                    f"  in stdout:\n{actual}"
                )

        # 4. Substring absence checks.
        for forbidden in case.get("stdout_not_contains", []):
            if forbidden in actual:
                raise ClingsError(
                    f"{ex['name']} case {case_no} found forbidden line:\n"
                    f"  forbidden substring: {forbidden!r}\n"
                    f"  in stdout:\n{actual}"
                )

        # 5. Regex check.
        pattern = case.get("stdout_regex")
        if pattern is not None and not re.search(pattern, actual):
            raise ClingsError(
                f"{ex['name']} case {case_no} regex not matched:\n"
                f"  pattern: {pattern!r}\n"
                f"  in stdout:\n{actual}"
            )


def check_return(ex: dict, binary: Path) -> None:
    """Verify the exit code of a compiled binary matches expected value."""
    expected = int(ex.get("expected_return", 0))
    stdin_text = ex.get("stdin", "")
    proc = subprocess.run(
        [str(binary)],
        input=stdin_text,
        text=True,
        capture_output=True,
        timeout=float(ex.get("timeout", 2.0)),
    )
    if proc.returncode != expected:
        raise ClingsError(
            f"{ex['name']}: expected return {expected}, got {proc.returncode}"
            + (f"\nstderr:\n{proc.stderr.strip()}" if proc.stderr.strip() else "")
        )


def check_one(
    ex: dict, use_solutions: bool, include_hidden: bool, use_cache: bool = True
) -> None:
    """Verify a single exercise (dispatch by mode).

    ``use_cache`` controls whether make-mode exercises consult the session-level
    MAKE_CACHE. Watch mode passes ``False`` so that every rerun triggers a real
    ``make`` invocation (make's own incremental build handles efficiency).
    Batch commands (``check``, ``score``) leave it ``True`` for acceleration.
    """
    mode = ex.get("mode", "stdout")
    if mode == "make":
        check_make(ex, use_solutions, use_cache)
        return
    if mode == "make+stdout":
        check_make_stdout(ex, use_solutions, include_hidden, use_cache)
        return
    if mode == "compile":
        compile_exercise(ex, use_solutions)
        return
    if mode == "return":
        binary = compile_exercise(ex, use_solutions)
        check_return(ex, binary)
        return
    binary = compile_exercise(ex, use_solutions)
    run_cases(ex, binary, include_hidden)


def check_make(ex: dict, use_solutions: bool, use_cache: bool = True) -> None:
    """Verify a make-based exercise by running its make targets."""
    src_dir = source_dir_for(ex, use_solutions)
    if not src_dir.exists():
        raise ClingsError(f"missing source directory: {src_dir.relative_to(ROOT)}")
    targets = ex.get("make_targets", ["test"])
    env = os.environ.copy()
    env.setdefault("CC", find_compiler() or "cc")
    mtime_sig = _make_source_mtime_signature(src_dir)
    for target in targets:
        cache_key = (str(src_dir.resolve()), target, use_solutions, mtime_sig)
        if use_cache and cache_key in MAKE_CACHE:
            continue
        cmd = ["make", target]
        proc = subprocess.run(
            cmd,
            cwd=src_dir,
            text=True,
            capture_output=True,
            timeout=float(ex.get("timeout", 120.0)),
            env=env,
        )
        if proc.returncode != 0:
            raise ClingsError(
                f"make target failed for {ex['name']}\n"
                f"$ {' '.join(cmd)} (cwd {src_dir})\n"
                f"{proc.stdout[-4000:]}\n{proc.stderr[-4000:]}"
            )
        if use_cache:
            MAKE_CACHE.add(cache_key)


def check_make_stdout(
    ex: dict, use_solutions: bool, include_hidden: bool, use_cache: bool = True
) -> None:
    """Hybrid mode: student Makefile builds; clings runs + compares stdout.

    Flow:
      1. `make <make_targets>` (default ["build"]) — student's Makefile compiles.
      2. Locate the produced binary (named via `binary` field or exercise name).
      3. clings runs the binary and compares stdout against bundled test cases
         (from clings/tests/<name>.toml or repo tests/<name>.toml).

    Students cannot tamper with the test cases: they live inside the installed
    clings package (site-packages) or repo-root tests/, never in the exercise
    directory. On failure, expected vs actual is shown so students can debug.
    """
    src_dir = source_dir_for(ex, use_solutions)
    if not src_dir.exists():
        raise ClingsError(f"missing source directory: {src_dir.relative_to(ROOT)}")

    # Phase 1: run student's Makefile targets (typically just `build`).
    targets = ex.get("make_targets", ["build"])
    env = os.environ.copy()
    env.setdefault("CC", find_compiler() or "cc")
    mtime_sig = _make_source_mtime_signature(src_dir)
    make_ran = False
    for target in targets:
        cache_key = (str(src_dir.resolve()), target, use_solutions, mtime_sig)
        if use_cache and cache_key in MAKE_CACHE:
            continue
        cmd = ["make", target]
        proc = subprocess.run(
            cmd,
            cwd=src_dir,
            text=True,
            capture_output=True,
            timeout=float(ex.get("timeout", 120.0)),
            env=env,
        )
        if proc.returncode != 0:
            raise ClingsError(
                f"make target failed for {ex['name']}\n"
                f"$ {' '.join(cmd)} (cwd {src_dir})\n"
                f"{proc.stdout[-4000:]}\n{proc.stderr[-4000:]}"
            )
        if use_cache:
            MAKE_CACHE.add(cache_key)
        make_ran = True

    # Phase 2: locate the produced binary.
    binary_name = ex.get("binary") or ex["name"]
    suffix = ".exe" if os.name == "nt" else ""
    binary = src_dir / f"{binary_name}{suffix}"
    if not binary.exists():
        if make_ran:
            raise ClingsError(
                f"make completed but did not produce expected binary: {binary_name}\n"
                f"looked at: {binary.relative_to(ROOT)}\n"
                f"check that your Makefile's TARGET matches the `binary` field "
                f"in exercises.toml."
            )
        raise ClingsError(
            f"expected binary not found: {binary_name}\n"
            f"looked at: {binary.relative_to(ROOT)}\n"
            f"make was skipped (cached from a previous run); the binary may have "
            f"been deleted externally.\n"
            f"Run `make clean` or press x to reset the exercise, then rerun."
        )

    # Phase 3: clings authoritative run + stdout comparison.
    run_cases(ex, binary, include_hidden)
