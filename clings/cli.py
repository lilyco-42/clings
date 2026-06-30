"""CLI entry point — argument parsing and command dispatch."""

import argparse

from .config import ClingsError, UNIT_LESSON_RANGES, get_version
from .commands.check import cmd_check
from .commands.doctor import cmd_doctor
from .commands.hint import cmd_hint
from .commands.init import cmd_init
from .commands.list import cmd_list
from .commands.reset import cmd_reset
from .commands.run import cmd_run
from .commands.score import cmd_score
from .commands.tests import cmd_tests
from .commands.watch import cmd_watch


def main() -> int:
    parser = argparse.ArgumentParser(
        prog="clings",
        description="Rustlings-style C exercises. Run without subcommand to enter watch mode.",
    )
    parser.add_argument("-v", "--version", action="version",
                        version=f"%(prog)s {get_version()}")
    sub = parser.add_subparsers(dest="command")

    p = sub.add_parser("list", help="list exercises with progress status")
    p.add_argument("selector", nargs="?")
    p.add_argument(
        "--check",
        action="store_true",
        help="re-verify all exercises before listing (slower but accurate)",
    )
    p.add_argument("--solutions", action="store_true")
    p.add_argument("--hidden", action="store_true")
    p.set_defaults(func=cmd_list)

    p = sub.add_parser("hint", help="show hint for an exercise (default: next pending)")
    p.add_argument("exercise", nargs="?")
    p.set_defaults(func=cmd_hint)

    p = sub.add_parser(
        "tests",
        help="tests for an exercise for an exercise (public tests only)",
    )
    p.add_argument(
        "exercise",
        nargs="?",
        metavar="exercise|selector",
        help="exercise name, lesson number, or unit (e.g. unit3). "
             "Omit to see a summary of all exercises.",
    )
    p.set_defaults(func=cmd_tests)

    p = sub.add_parser("run", help="run exercise(s): name, unit selector, 'random', or next pending")
    p.add_argument("exercise", nargs="?", metavar="exercise|selector",
                   help="exercise name, unit (unit0/unit1/unit2), lesson number, or 'random'")
    p.add_argument("--solutions", action="store_true")
    p.add_argument("--hidden", action="store_true")
    p.set_defaults(func=cmd_run)

    p = sub.add_parser("check", help="batch verify exercises")
    p.add_argument("selector", nargs="?")
    p.add_argument("--solutions", action="store_true")
    p.add_argument("--hidden", action="store_true")
    p.set_defaults(func=cmd_check)

    p = sub.add_parser("score", help="run all exercises and output scoring report for CI")
    p.add_argument("selector", nargs="?")
    p.add_argument("--solutions", action="store_true",
                   help="use solutions directory instead of exercises")
    p.add_argument("--hidden", action="store_true",
                   help="include hidden test cases")
    p.add_argument("--output", "-o", metavar="FILE",
                   help="output JSON report path (default: clings_score.json)")
    p.add_argument("--json", action="store_true",
                   help="also print JSON report to stdout")
    p.set_defaults(func=cmd_score)

    p = sub.add_parser("watch", help="interactive watch mode (default when no subcommand)")
    p.add_argument("selector", nargs="?")
    p.add_argument("--solutions", action="store_true")
    p.add_argument("--hidden", action="store_true")
    p.add_argument("--manual-run", action="store_true",
                   help="disable auto-rerun on file change; press r to rerun")
    p.add_argument("--auto-advance", action="store_true",
                   help="automatically advance to the next pending exercise after "
                        "the current one passes (shows a brief success screen first)")
    p.add_argument("--edit-cmd", metavar="CMD",
                   help="command to open exercise file (e.g. 'code' or 'vim')")
    p.set_defaults(func=cmd_watch)

    p = sub.add_parser("reset", help="reset exercise file or all progress")
    p.add_argument("exercise", help="exercise name or 'progress' to reset all progress")
    p.set_defaults(func=cmd_reset)

    p = sub.add_parser("init", help="initialize exercises (default: unit1)")
    available_units = ", ".join(list(UNIT_LESSON_RANGES.keys()) + ["all"])
    p.add_argument("unit", nargs="?", default="unit1",
                   help=f"unit to initialize: {available_units} (default: unit1)")
    p.set_defaults(func=cmd_init)

    p = sub.add_parser("doctor", help="check environment")
    p.set_defaults(func=cmd_doctor)

    args = parser.parse_args()

    if args.command is None:
        args.selector = None
        args.solutions = False
        args.hidden = False
        args.manual_run = False
        args.auto_advance = False
        args.func = cmd_watch

    try:
        return args.func(args)
    except KeyboardInterrupt:
        return 130
    except ClingsError as exc:
        print(exc, file=__import__("sys").stderr)
        return 1
