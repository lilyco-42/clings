"""clings run — run exercise(s) and show their actual output.

Unlike ``clings check`` (which grades silently and bails on the first failing
exercise), ``clings run`` is the student-facing "what does my program actually
print?" command: it builds the exercise, streams the program's stdout/stderr to
the terminal, and then reports pass/fail.

`run` and `check` are deliberately separate commands with different jobs, but
their pass/fail *verdict* is decided by the very same
:func:`clings.compiler.assert_case`. That keeps them from ever disagreeing about
whether an exercise passes, while ``run`` layers the "show me the output"
behavior on top — the previous implementation carried a second, weaker copy of
the assertion logic that ignored ``make+stdout`` builds and every stdout field
except an exact match.
"""

import argparse
import os
import subprocess
import sys

_IS_WINDOWS = os.name == "nt"
_OK = "ok" if _IS_WINDOWS else "\u2705"
_FAILED = "FAILED" if _IS_WINDOWS else "\u274c"

from ..compiler import (
    _collect_cases,
    _run,
    assert_case,
    compile_exercise,
)
from ..config import (
    ClingsError,
    exercises,
    find_compiler,
    find_exercise,
    load_config,
    select_exercises,
    source_dir_for,
)
from ..state import WatchState, next_pending_exercise


def _run_and_show_cases(ex: dict, binary, include_hidden: bool) -> int:
    """Run every case against ``binary``, stream output, verify via assert_case.

    Shared by the ``stdout`` and ``make+stdout`` modes — they differ only in how
    the binary is produced, not in how it is exercised. Returns 0 if all cases
    pass, 1 on the first failing case (output for that case is shown first).
    """
    all_cases = _collect_cases(ex, include_hidden)
    if not all_cases:
        raise ClingsError(f"no test cases found for {ex['name']}")

    runnable = [c for c in all_cases if not c.get("compile_only", False)]
    total = len(runnable)

    for idx, case in enumerate(runnable, 1):
        stdin_text = case.get("stdin", "")
        case_args = [str(a) for a in case.get("args", [])]
        timeout = float(case.get("timeout", 2.0))

        # With multiple cases, label each so the student knows which inputs
        # produced which output.
        if total > 1:
            label = f"case {idx}/{total}"
            if case_args:
                label += f" args=[{' '.join(case_args)}]"
            if stdin_text:
                preview = stdin_text.replace("\n", "\\n")
                if len(preview) > 40:
                    preview = preview[:37] + "..."
                label += f' stdin="{preview}"'
            sys.stdout.flush()
            sys.stderr.flush()
            print(f"\x1b[90m[{label}]\x1b[0m", flush=True)

        try:
            proc = _run([str(binary), *case_args], input=stdin_text, timeout=timeout)
        except subprocess.TimeoutExpired:
            print(
                f"\n\x1b[31;1m{_FAILED} {ex['name']} case {idx} timed out after "
                f"{timeout}s (possible infinite loop)\x1b[0m",
                file=sys.stderr,
            )
            return 1

        # Showing the program's real output is the whole point of `run`.
        if proc.stdout:
            print(proc.stdout, end="", flush=True)
        if proc.stderr.strip():
            print(f"\x1b[33m{proc.stderr.strip()}\x1b[0m", flush=True)

        # Verdict via the shared grading core: covers exit_code, exact stdout,
        # stdout_b64, stdout_contains / stdout_not_contains, stdout_regex and
        # trim_trailing_ws — identical to what `clings check` enforces.
        try:
            assert_case(
                case, proc.stdout, proc.returncode,
                name=ex["name"], case_no=idx, stderr=proc.stderr,
            )
        except ClingsError as exc:
            print(f"\n\x1b[31;1m{_FAILED} {ex['name']} case {idx} FAILED\x1b[0m",
                  file=sys.stderr)
            print(str(exc), file=sys.stderr)
            return 1

    print(f"\n\x1b[32;1m{_OK} ok {ex['name']}\x1b[0m")
    return 0


def _run_one(ex: dict, use_solutions: bool, include_hidden: bool) -> int:
    """Run a single exercise: build it (per mode), show output, and verify."""
    mode = ex.get("mode", "stdout")

    # ── make-based modes: build with the student's own Makefile ──────────────
    if mode in ("make", "make+stdout"):
        src_dir = source_dir_for(ex, use_solutions)
        if not src_dir.exists():
            raise ClingsError(f"missing source directory: {src_dir}")
        default_targets = ["all"] if mode == "make+stdout" else ["test"]
        targets = ex.get("make_targets", default_targets)
        env = os.environ.copy()
        env.setdefault("CC", find_compiler() or "cc")
        timeout = float(ex.get("timeout", 120.0))
        for target in targets:
            try:
                proc = _run(["make", target], cwd=src_dir, timeout=timeout, env=env)
            except subprocess.TimeoutExpired:
                print(f"\n\x1b[31;1m{_FAILED} {ex['name']} make {target} timed out "
                      f"after {timeout}s\x1b[0m", file=sys.stderr)
                return 1
            if proc.returncode != 0:
                # Surface the build output so students can fix compile errors.
                if proc.stdout.strip():
                    print(proc.stdout, end="", flush=True)
                if proc.stderr.strip():
                    print(f"\x1b[33m{proc.stderr.strip()}\x1b[0m", flush=True)
                print(f"\n\x1b[31;1m{_FAILED} {ex['name']} make {target} FAILED\x1b[0m",
                      file=sys.stderr)
                return 1

        # `make` mode: build-only, there is no program output to show.
        if mode == "make":
            print(f"\n\x1b[32;1m{_OK} ok {ex['name']}\x1b[0m")
            return 0

        # `make+stdout` mode: locate the produced binary, then run its cases.
        binary_name = ex.get("binary") or ex["name"]
        suffix = ".exe" if os.name == "nt" else ""
        binary = src_dir / f"{binary_name}{suffix}"
        if not binary.exists():
            raise ClingsError(
                f"make completed but did not produce expected binary: {binary_name}\n"
                f"looked at: {binary}\n"
                f"check that your Makefile's TARGET matches the `binary` field "
                f"in exercises.toml."
            )
        return _run_and_show_cases(ex, binary, include_hidden)

    # ── compile-based modes: clings compiles the sources itself ──────────────
    binary = compile_exercise(ex, use_solutions)

    if mode == "compile":
        print(f"\x1b[32;1m{_OK} ok {ex['name']} (compiled successfully)\x1b[0m")
        return 0

    if mode == "return":
        expected = int(ex.get("expected_return", 0))
        stdin_text = ex.get("stdin", "")
        timeout = float(ex.get("timeout", 2.0))
        try:
            proc = _run([str(binary)], input=stdin_text, timeout=timeout)
        except subprocess.TimeoutExpired:
            print(f"\n\x1b[31;1m{_FAILED} {ex['name']} timed out after {timeout}s\x1b[0m",
                  file=sys.stderr)
            return 1
        if proc.stdout:
            print(proc.stdout, end="")
        print(f"\n\x1b[90m(exit code: {proc.returncode})\x1b[0m")
        if proc.returncode != expected:
            print(f"\x1b[31;1m{_FAILED} {ex['name']}: expected return {expected}, "
                  f"got {proc.returncode}\x1b[0m", file=sys.stderr)
            return 1
        print(f"\x1b[32;1m{_OK} ok {ex['name']}\x1b[0m")
        return 0

    # mode == "stdout": clings-compiled binary, run and show all cases.
    return _run_and_show_cases(ex, binary, include_hidden)


def _is_selector(value: str) -> bool:
    """Check if value is a batch selector (unit/lesson) rather than an exercise name."""
    if value.startswith("unit"):
        return True
    if value.isdigit():
        return True
    return False


def cmd_run(args: argparse.Namespace) -> int:
    import random as _random

    config = load_config()
    selector = args.exercise

    # 批量模式：selector 为 unit0/unit1/unit2 或 lesson 号
    if selector and _is_selector(selector):
        selected = select_exercises(config, selector)
        total = len(selected)
        for index, ex in enumerate(selected, 1):
            print(f"\x1b[1m--- [{index}/{total}] {ex['name']} ---\x1b[0m", flush=True)
            if _run_one(ex, args.solutions, args.hidden) != 0:
                return 1
        print(f"\n\x1b[32;1m{_OK} all {total} exercise(s) passed\x1b[0m")
        return 0

    # 单题模式
    if not selector or selector == "next":
        ex = next_pending_exercise(config)
        if ex is None:
            print("all exercises completed!")
            return 0
    elif selector == "random":
        all_ex = exercises(config)
        state = WatchState(all_ex)
        pending = [e for e in all_ex if not state.is_done(e)]
        if not pending:
            pending = all_ex
        ex = _random.choice(pending)
    else:
        ex = find_exercise(config, selector)

    return _run_one(ex, args.solutions, args.hidden)
