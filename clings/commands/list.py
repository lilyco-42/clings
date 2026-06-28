"""clings list — show exercises with progress status.

By default, reads the progress state file (`.clings-state.txt`) populated by
`clings check` and `clings watch`. This is fast (no compilation).

Use `--check` to force a real verification pass before listing: each exercise
is compiled and tested, and the progress state is updated to reflect the
actual pass/fail status. This is slower but gives a ground-truth view.

Usage:
    clings list              # fast: show saved progress
    clings list unit3        # fast: filter by unit
    clings list --check      # slow: re-verify all, then show
    clings list unit3 --check  # slow: re-verify unit3, then show
"""

import argparse
import sys

from ..config import STATE_FILE, exercises, load_config, select_exercises
from ..state import WatchState
from ..utils import progress_bar


def cmd_list(args: argparse.Namespace) -> int:
    config = load_config()
    selected = select_exercises(config, args.selector)
    all_ex = exercises(config)

    # If --check is requested, run a verification pass and sync the state.
    if args.check:
        from ..compiler import check_one
        state = WatchState(all_ex)
        for ex in selected:
            try:
                check_one(ex, args.solutions, args.hidden)
            except Exception:
                # Failed: ensure it's NOT marked done.
                state.mark_pending(ex["name"])
                continue
            # Passed: mark done.
            if state.jump_to(ex["name"]):
                state.mark_done()
        state.save()

    # Read (possibly updated) state for display.
    done_names: set[str] = set()
    if STATE_FILE.exists():
        state = WatchState(all_ex)
        done_names = state._done

    n_done = sum(1 for ex in selected if ex["name"] in done_names)
    for ex in selected:
        mode = ex.get("mode", "stdout")
        marker = "\x1b[32m\u2714\x1b[0m" if ex["name"] in done_names else "\x1b[31m\u2022\x1b[0m"
        print(f"  {marker} {ex['unit']:5} {ex['lesson']:02d} {mode:8} {ex['name']:32} {ex['title']}")
    total = len(selected)
    print(f"\n  {progress_bar(n_done, total)}")
    return 0
