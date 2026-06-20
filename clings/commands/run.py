"""clings run — run a single exercise."""

import argparse
import subprocess
import sys

from ..compiler import (
    ClingsError,
    _collect_cases,
    check_make,
    compile_exercise,
    normalize,
)
from ..config import exercises, find_exercise, load_config
from ..state import WatchState, next_pending_exercise


def cmd_run(args: argparse.Namespace) -> int:
    import random as _random

    config = load_config()
    if not args.exercise or args.exercise == "next":
        ex = next_pending_exercise(config)
        if ex is None:
            print("all exercises completed!")
            return 0
    elif args.exercise == "random":
        all_ex = exercises(config)
        pending = [e for e in all_ex if not WatchState(all_ex).is_done(e)]
        if not pending:
            pending = all_ex
        ex = _random.choice(pending)
    else:
        ex = find_exercise(config, args.exercise)
    mode = ex.get("mode", "stdout")
    use_solutions = args.solutions

    if mode == "make":
        # run 模式: 展示 make 执行过程（不捕获输出，直接流向终端）
        from ..compiler import source_dir_for, find_compiler
        import os
        src_dir = source_dir_for(ex, use_solutions)
        targets = ex.get("make_targets", ["test"])
        env = os.environ.copy()
        env.setdefault("CC", find_compiler() or "cc")
        for target in targets:
            proc = subprocess.run(
                ["make", target], cwd=src_dir, text=True,
                timeout=float(ex.get("timeout", 120.0)), env=env,
            )
            if proc.returncode != 0:
                print(f"\n\x1b[31;1m\u274c {ex['name']} FAILED\x1b[0m", file=sys.stderr)
                return 1
        print(f"\n\x1b[32;1m\u2705 ok {ex['name']}\x1b[0m")
        return 0

    binary = compile_exercise(ex, use_solutions)

    if mode == "compile":
        print(f"\x1b[32;1m\u2705 ok {ex['name']} (compiled successfully)\x1b[0m")
        return 0

    if mode == "return":
        expected = int(ex.get("expected_return", 0))
        stdin_text = ex.get("stdin", "")
        proc = subprocess.run(
            [str(binary)], input=stdin_text, text=True,
            capture_output=True, timeout=float(ex.get("timeout", 2.0)),
        )
        if proc.stdout:
            print(proc.stdout, end="")
        print(f"\n\x1b[90m(exit code: {proc.returncode})\x1b[0m")
        if proc.returncode != expected:
            print(f"\x1b[31;1m\u274c {ex['name']}: expected return {expected}, got {proc.returncode}\x1b[0m")
            return 1
        print(f"\x1b[32;1m\u2705 ok {ex['name']}\x1b[0m")
        return 0

    # mode == "stdout": run and show ALL cases, verify each one
    all_cases = _collect_cases(ex, args.hidden)
    if not all_cases:
        raise ClingsError(f"no test cases found for {ex['name']}")

    runnable = [c for c in all_cases if not c.get("compile_only", False)]
    total = len(runnable)

    for idx, case in enumerate(runnable, 1):
        stdin_text = case.get("stdin", "")
        expected_stdout = case.get("stdout", "")
        expected_exit = int(case.get("exit_code", 0))
        case_args = [str(a) for a in case.get("args", [])]
        timeout = float(case.get("timeout", 2.0))

        # 标注 case 序号（多 case 时才显示）
        if total > 1:
            args_str = " ".join(case_args) if case_args else ""
            label = f"case {idx}/{total}"
            if args_str:
                label += f" args=[{args_str}]"
            if stdin_text:
                preview = stdin_text.replace("\n", "\\n")
                if len(preview) > 40:
                    preview = preview[:37] + "..."
                label += f' stdin="{preview}"'
            sys.stdout.flush()
            sys.stderr.flush()
            print(f"\x1b[90m[{label}]\x1b[0m", flush=True)

        proc = subprocess.run(
            [str(binary)] + case_args, input=stdin_text, text=True,
            capture_output=True, timeout=timeout,
        )

        # 展示输出（stdout 和 stderr 统一输出到 stdout，保持顺序）
        if proc.stdout:
            print(proc.stdout, end="", flush=True)
        if proc.stderr.strip():
            print(f"\x1b[33m{proc.stderr.strip()}\x1b[0m", flush=True)

        # 验证 exit code
        if proc.returncode != expected_exit:
            print(
                f"\n\x1b[31;1m\u274c {ex['name']} case {idx} FAILED\x1b[0m"
                f" (exit {proc.returncode}, expected {expected_exit})",
                file=sys.stderr,
            )
            return 1

        # 验证 stdout（如果定义了期望值）
        if expected_stdout and normalize(proc.stdout) != normalize(expected_stdout):
            print(
                f"\n\x1b[31;1m\u274c {ex['name']} case {idx} output mismatch\x1b[0m\n"
                f"expected:\n{expected_stdout}"
                f"actual:\n{proc.stdout}",
                file=sys.stderr,
            )
            return 1

    print(f"\n\x1b[32;1m\u2705 ok {ex['name']}\x1b[0m")
    return 0
