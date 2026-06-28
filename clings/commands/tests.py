"""clings tests — display test cases for an exercise.

Students can inspect the expected test cases (public tests only) to understand
what their program should produce. Hidden tests are never shown.

Usage:
    clings show 49                  # by lesson number
    clings show dining              # by name prefix
    clings show 49_dining-philosophers-sync  # by full name
    clings show unit3               # show summary for all exercises in unit3
"""

import argparse
import base64
import sys
from pathlib import Path

from ..config import (
    ClingsError,
    exercises,
    find_exercise,
    load_config,
    select_exercises,
    test_files_for,
)
from ..compiler import _collect_cases


# ANSI color helpers (keep consistent with other commands)
_DIM = "\x1b[2m"
_BOLD = "\x1b[1m"
_GREEN = "\x1b[32m"
_CYAN = "\x1b[36m"
_YELLOW = "\x1b[33m"
_RESET = "\x1b[0m"


def _format_case(case: dict, idx: int, total: int) -> str:
    """Format a single test case for display."""
    lines = [f"{_BOLD}[Case {idx}/{total}]{_RESET}"]

    # Stdin (if any)
    stdin_text = case.get("stdin", "")
    if stdin_text:
        preview = stdin_text
        if len(preview) > 200:
            preview = preview[:197] + "..."
        lines.append(f"  {_DIM}stdin:{_RESET}")
        for line in preview.split("\n"):
            lines.append(f"    {line}")

    # Args (if any)
    args_list = case.get("args", [])
    if args_list:
        lines.append(f"  {_DIM}args:{_RESET} {args_list}")

    # Expected exit code (if non-zero)
    exit_code = case.get("exit_code", 0)
    if exit_code != 0:
        lines.append(f"  {_DIM}exit code:{_RESET} {exit_code}")

    # Compile-only flag
    if case.get("compile_only", False):
        lines.append(f"  {_YELLOW}(compile-only: just needs to compile){_RESET}")
        return "\n".join(lines)

    # Expected stdout
    if "stdout_b64" in case:
        try:
            expected = base64.b64decode(case["stdout_b64"]).decode(
                "utf-8", errors="replace"
            )
        except Exception:
            expected = "(invalid base64 data)"
        lines.append(f"  {_DIM}expected stdout (base64-decoded):{_RESET}")
    else:
        expected = case.get("stdout", "")
        lines.append(f"  {_DIM}expected stdout:{_RESET}")

    if expected:
        for line in expected.split("\n"):
            lines.append(f"    {_CYAN}{line}{_RESET}")
    else:
        lines.append(f"    {_DIM}(empty){_RESET}")

    return "\n".join(lines)


def _show_one(ex: dict) -> int:
    """Show test cases for a single exercise."""
    name = ex["name"]
    title = ex.get("title", name)

    # Collect only PUBLIC test cases (include_hidden=False)
    cases = _collect_cases(ex, include_hidden=False)

    print(f"{_BOLD}=== {title} ==={_RESET}")
    print(f"{_DIM}name: {name}{_RESET}")
    print(f"{_DIM}unit: {ex.get('unit', '?')}  lesson: {ex.get('lesson', '?')}{_RESET}")
    print(f"{_DIM}mode: {ex.get('mode', 'stdout')}{_RESET}")

    if not cases:
        print(f"\n{_YELLOW}(no public test cases available){_RESET}")
        print(f"{_DIM}This exercise may use compile-only mode or hidden tests.{_RESET}")
        return 0

    # Filter out compile-only cases for counting display purposes
    runnable = [c for c in cases if not c.get("compile_only", False)]
    compile_only = [c for c in cases if c.get("compile_only", False)]

    # Build the count descriptor
    parts = [f"{len(cases)} total"]
    if runnable:
        parts.append(f"{len(runnable)} runnable")
    if compile_only:
        parts.append(f"{len(compile_only)} compile-only")
    count_desc = ", ".join(parts)

    print(f"\n{_BOLD}Test Cases ({count_desc}):{_RESET}\n")

    for idx, case in enumerate(cases, 1):
        print(_format_case(case, idx, len(cases)))
        if idx < len(cases):
            print()

    return 0


def _show_summary(selected: list[dict]) -> int:
    """Show a summary table of test cases for multiple exercises."""
    print(f"{_BOLD}{'Lesson':<8} {'Name':<40} {'Mode':<14} {'Cases':<8} Title{_RESET}")
    print("-" * 100)

    total_cases = 0
    for ex in selected:
        cases = _collect_cases(ex, include_hidden=False)
        n = len(cases)
        total_cases += n
        lesson = str(ex.get("lesson", "?"))
        name = ex["name"][:38]
        mode = ex.get("mode", "stdout")
        title = ex.get("title", "")[:40]
        print(f"{lesson:<8} {name:<40} {mode:<14} {n:<8} {title}")

    print("-" * 100)
    print(f"{_BOLD}Total: {len(selected)} exercises, {total_cases} test cases{_RESET}")
    return 0


def cmd_tests(args: argparse.Namespace) -> int:
    """Entry point for `clings show`."""
    config = load_config()
    selector = args.exercise

    if not selector:
        # No selector: show summary of all exercises
        all_ex = exercises(config)
        if not all_ex:
            print("No exercises found.")
            return 0
        return _show_summary(all_ex)

    # Check if it's a batch selector (unit or lesson range)
    if selector.startswith("unit"):
        selected = select_exercises(config, selector)
        if not selected:
            print(f"No exercises found for {selector}")
            return 1
        return _show_summary(selected)

    # Single exercise lookup
    try:
        ex = find_exercise(config, selector)
    except ClingsError as exc:
        print(exc, file=sys.stderr)
        return 1

    return _show_one(ex)
