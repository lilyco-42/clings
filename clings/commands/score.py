"""clings score — run all exercises and generate a scoring report for CI grading."""

import argparse
import json
import os
import sys
from pathlib import Path

from ..compiler import check_one
from ..config import ROOT, SOLUTIONS_ENV, load_config, select_exercises


# Default output file name (compatible with c-training-advanced format)
DEFAULT_OUTPUT_FILE = "clings_score.json"


def cmd_score(args: argparse.Namespace) -> int:
    config = load_config()
    selected = select_exercises(config, args.selector)

    if not selected:
        print("no exercises match the selector", file=sys.stderr)
        return 1

    use_solutions = args.solutions
    include_hidden = args.hidden
    output_path = Path(args.output) if args.output else ROOT / DEFAULT_OUTPUT_FILE

    # Validate solutions directory if --solutions is used
    if use_solutions and not Path(os.environ.get(SOLUTIONS_ENV, ROOT / "solutions")).exists():
        print(
            f"solutions are hidden; set {SOLUTIONS_ENV} to the private solutions directory",
            file=sys.stderr,
        )
        return 1

    total = len(selected)
    passed = 0
    failed = 0
    exercise_results: list[dict] = []

    # Run every exercise, never bail early — we need full coverage for scoring
    for index, ex in enumerate(selected, 1):
        name = ex["name"]
        label = f"[{index}/{total}] {name}"
        try:
            check_one(ex, use_solutions, include_hidden)
            exercise_results.append({
                "name": name,
                "status": "PASSED",
                "score": 1,
            })
            passed += 1
            print(f"{label} \x1b[32mPASSED\x1b[0m (+1)")
        except Exception as exc:
            error_msg = str(exc)
            # Determine status: NOT_COMPLETED if source still has the scaffold
            # "#error TODO" marker (student hasn't started this exercise).
            status = _detect_status(ex, use_solutions, error_msg)
            exercise_results.append({
                "name": name,
                "status": status,
                "error": error_msg[:500],  # Truncate long errors for JSON
                "score": 0,
            })
            failed += 1
            status_label = "\x1b[33mNOT_COMPLETED\x1b[0m" if status == "NOT_COMPLETED" else "\x1b[31mFAILED\x1b[0m"
            print(f"{label} {status_label} (+0)")

    # Build the report (compatible with c-training-advanced test_results_summary.json)
    report = {
        "test_summary": {
            "total_exercises": total,
            "passed_exercises": passed,
            "failed_exercises": failed,
            "total_score": passed,
        },
        "exercises": exercise_results,
    }

    # Write JSON report to file
    try:
        output_path.write_text(
            json.dumps(report, indent=2, ensure_ascii=False) + "\n",
            encoding="utf-8",
        )
    except OSError as exc:
        print(f"warning: failed to write report to {output_path}: {exc}", file=sys.stderr)

    # Print summary
    print(f"\n{'=' * 50}")
    print(f"  Total: {total}  Passed: {passed}  Failed: {failed}")
    print(f"  Score: {passed}/{total}")
    if output_path.exists():
        print(f"  Report: {output_path}")
    print(f"{'=' * 50}")

    # Output for CNB CI: ##[set-output score=N]
    print(f"##[set-output score={passed}]")

    # Also output JSON to stdout if --json flag is set
    if args.json:
        print(json.dumps(report, indent=2, ensure_ascii=False))

    return 0


# Scaffold marker that clings templates place at every TODO. Its presence means
# the student hasn't finished that spot yet — and because it is a `#error`
# directive, the source won't even compile until every one is removed. (Earlier
# code looked for rustlings' "I AM NOT DONE" comment, which clings never emits,
# so NOT_COMPLETED could never trigger.)
NOT_DONE_MARKER = "#error TODO"


def _detect_status(ex: dict, use_solutions: bool, error_msg: str) -> str:
    """Detect whether a failed exercise is NOT_COMPLETED or truly FAILED.

    NOT_COMPLETED: the student hasn't started working on it yet — a source file
    still contains the scaffold ``#error TODO`` marker.
    FAILED: the student attempted it (all markers removed) but the code doesn't
    compile or its output doesn't match.
    """
    from ..utils import source_files_for

    src_files = source_files_for(ex, use_solutions)
    for src_file in src_files:
        if not src_file.exists():
            continue
        try:
            content = src_file.read_text(encoding="utf-8", errors="replace")
            if NOT_DONE_MARKER in content:
                return "NOT_COMPLETED"
        except OSError:
            continue

    return "FAILED"
