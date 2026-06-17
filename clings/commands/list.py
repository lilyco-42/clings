"""clings list — show exercises with progress status."""

import argparse

from ..config import STATE_FILE, exercises, load_config, select_exercises
from .watch import WatchState, _progress_bar


def cmd_list(args: argparse.Namespace) -> int:
    config = load_config()
    selected = select_exercises(config, args.selector)
    all_ex = exercises(config)
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
    print(f"\n  {_progress_bar(n_done, total)}")
    return 0
