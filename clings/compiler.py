"""Compilation, execution, and test verification logic."""

import os
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

# Cache for make targets already verified in this session
MAKE_CACHE: set[tuple[str, str, bool]] = set()


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


def normalize(text: str) -> str:
    """Normalize line endings for comparison."""
    return text.replace("\r\n", "\n")


def _collect_cases(ex: dict, include_hidden: bool) -> list[dict]:
    """Collect test cases from inline exercise data and/or external test files."""
    cases = list(ex.get("cases", []))
    for test_file in test_files_for(ex, include_hidden):
        data = load_toml(test_file)
        cases.extend(data.get("cases", []))
    return cases


def run_cases(ex: dict, binary: Path, include_hidden: bool) -> None:
    """Run all test cases for an exercise against the compiled binary."""
    cases = _collect_cases(ex, include_hidden)
    if not cases:
        raise ClingsError(f"no test cases found for {ex['name']}")
    case_no = 0
    for case in cases:
        case_no += 1
        if case.get("compile_only", False):
            continue
        stdin = case.get("stdin", "")
        expected = case.get("stdout", "")
        args = [str(arg) for arg in case.get("args", [])]
        timeout = float(case.get("timeout", 2.0))
        proc = subprocess.run(
            [str(binary), *args],
            input=stdin,
            text=True,
            capture_output=True,
            timeout=timeout,
        )
        actual = normalize(proc.stdout)
        if proc.returncode != int(case.get("exit_code", 0)):
            raise ClingsError(
                f"{ex['name']} case {case_no} exited {proc.returncode}\n"
                f"stderr:\n{proc.stderr.strip()}"
            )
        if actual != normalize(expected):
            raise ClingsError(
                f"{ex['name']} case {case_no} output mismatch\n"
                f"stdin:\n{stdin}"
                f"expected:\n{expected}"
                f"actual:\n{actual}"
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


def check_one(ex: dict, use_solutions: bool, include_hidden: bool) -> None:
    """Verify a single exercise (dispatch by mode)."""
    mode = ex.get("mode", "stdout")
    if mode == "make":
        check_make(ex, use_solutions)
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


def check_make(ex: dict, use_solutions: bool) -> None:
    """Verify a make-based exercise by running its make targets."""
    src_dir = source_dir_for(ex, use_solutions)
    if not src_dir.exists():
        raise ClingsError(f"missing source directory: {src_dir.relative_to(ROOT)}")
    targets = ex.get("make_targets", ["test"])
    env = os.environ.copy()
    env.setdefault("CC", find_compiler() or "cc")
    for target in targets:
        cache_key = (str(src_dir.resolve()), target, use_solutions)
        if cache_key in MAKE_CACHE:
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
        MAKE_CACHE.add(cache_key)
