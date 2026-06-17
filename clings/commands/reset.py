"""clings reset — reset exercise files or progress."""

import argparse
import sys

from ..config import STATE_FILE, find_exercise, load_config, select_exercises
from ..state import WatchState
from ..utils import reset_exercise


def cmd_reset(args: argparse.Namespace) -> int:
    config = load_config()
    if args.exercise == "progress":
        if STATE_FILE.exists():
            STATE_FILE.unlink()
            print("progress reset — all exercises marked pending")
        else:
            print("no progress file found")
        return 0
    ex = find_exercise(config, args.exercise)
    if reset_exercise(ex):
        print(f"reset {ex['name']} — file restored from git")
        if STATE_FILE.exists():
            selected = select_exercises(config, None)
            state = WatchState(selected)
            state.mark_pending(ex["name"])
            print(f"  (marked as pending)")
    else:
        print(f"reset failed for {ex['name']} — git checkout failed", file=sys.stderr)
        return 1
    return 0
