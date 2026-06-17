"""clings hint — show hint for an exercise."""

import argparse

from ..config import find_exercise, load_config
from ..state import next_pending_exercise


def cmd_hint(args: argparse.Namespace) -> int:
    config = load_config()
    if args.exercise:
        ex = find_exercise(config, args.exercise)
    else:
        ex = next_pending_exercise(config)
        if ex is None:
            print("all exercises completed!")
            return 0
    print(ex.get("hint", "No hint yet."))
    return 0
