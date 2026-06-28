"""clings check — batch verify exercises.

When an exercise passes, it is automatically marked as done in the progress
state file (`.clings-state.txt`), so that `clings list` reflects the real
completion status without requiring the student to manually press a key in
watch mode.

If `--solutions` is used (CI / maintainer mode), the progress state is left
untouched — solutions are reference answers, not student submissions.
"""

import argparse
import os
import sys
from pathlib import Path

from ..compiler import check_one
from ..config import ROOT, SOLUTIONS_ENV, exercises, load_config, select_exercises
from ..state import WatchState


def cmd_check(args: argparse.Namespace) -> int:
    config = load_config()
    selected = select_exercises(config, args.selector)
    if args.solutions and not Path(os.environ.get(SOLUTIONS_ENV, ROOT / "solutions")).exists():
        print(
            f"solutions are hidden; set {SOLUTIONS_ENV} to the private solutions directory",
            file=sys.stderr,
        )
        return 1

    # Load watch state once for marking exercises done.
    # Only maintain state when checking student code (not --solutions),
    # because solutions are reference answers, not student progress.
    state: WatchState | None = None
    if not args.solutions:
        all_ex = exercises(config)
        state = WatchState(all_ex)

    total = len(selected)
    passed = 0
    failed = 0
    for index, ex in enumerate(selected, 1):
        label = f"[{index}/{total}] {ex['name']}"
        try:
            check_one(ex, args.solutions, args.hidden)
        except Exception as exc:
            print(f"{label} FAILED\n{exc}", file=sys.stderr)
            failed += 1
            # Ensure failed exercise is NOT marked done.
            if state is not None:
                state.mark_pending(ex["name"])
            continue
        print(f"{label} ok")
        passed += 1
        # Auto-mark done in student progress state.
        # WatchState.mark_done operates on current_exercise, so we jump first.
        if state is not None:
            if state.jump_to(ex["name"]):
                state.mark_done()

    # Persist state once at the end (mark_done already saves, but this is
    # a safety net in case any edge case skipped save).
    if state is not None and passed > 0:
        state.save()

    if failed > 0:
        print(f"{passed}/{total} passed, {failed} failed", file=sys.stderr)
        return 1
    print(f"all {total} exercise(s) passed")
    return 0
