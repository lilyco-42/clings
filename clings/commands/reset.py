"""clings reset — reset exercise files or progress."""

import argparse
import subprocess
import sys

from ..config import ROOT, STATE_FILE, find_exercise, load_config, select_exercises, source_dir_for
from .watch import WatchState


def _reset_exercise(ex: dict) -> bool:
    """Reset an exercise file via git checkout."""
    src_dir = source_dir_for(ex, use_solutions=False)
    if not src_dir.exists():
        return False
    try:
        result = subprocess.run(
            ["git", "checkout", "--", str(src_dir)],
            cwd=ROOT, text=True, capture_output=True,
        )
        return result.returncode == 0
    except FileNotFoundError:
        return False


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
    if _reset_exercise(ex):
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
