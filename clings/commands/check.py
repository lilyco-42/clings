"""clings check — batch verify exercises."""

import argparse
import os
import sys
from pathlib import Path

from ..compiler import check_one
from ..config import ROOT, SOLUTIONS_ENV, load_config, select_exercises


def cmd_check(args: argparse.Namespace) -> int:
    config = load_config()
    selected = select_exercises(config, args.selector)
    if args.solutions and not Path(os.environ.get(SOLUTIONS_ENV, ROOT / "solutions")).exists():
        print(
            f"solutions are hidden; set {SOLUTIONS_ENV} to the private solutions directory",
            file=sys.stderr,
        )
        return 1
    total = len(selected)
    for index, ex in enumerate(selected, 1):
        label = f"[{index}/{total}] {ex['name']}"
        try:
            check_one(ex, args.solutions, args.hidden)
        except Exception as exc:
            print(f"{label} FAILED\n{exc}", file=sys.stderr)
            return 1
        print(f"{label} ok")
    print(f"all {total} exercise(s) passed")
    return 0
