"""clings watch — interactive watch mode with file change detection."""

import argparse
import os
import subprocess
import sys
import time

from ..compiler import check_one
from ..config import ROOT, load_config, select_exercises
from ..renderer import WatchRenderer
from ..state import WatchState
from ..utils import (
    ANSI_BOLD,
    ANSI_DIM,
    ANSI_GREEN,
    ANSI_RED,
    ANSI_RESET,
    get_mtime,
    progress_bar,
    read_key_nonblocking,
    reset_exercise,
    source_files_for,
)


def cmd_watch(args: argparse.Namespace) -> int:
    config = load_config()
    selected = select_exercises(config, args.selector)
    if not selected:
        print("no exercises match the selector", file=sys.stderr)
        return 1

    state = WatchState(selected)
    renderer = WatchRenderer(state)
    use_solutions = args.solutions
    include_hidden = args.hidden
    manual_run = getattr(args, "manual_run", False)
    edit_cmd = getattr(args, "edit_cmd", None) or os.environ.get("CLINGS_EDITOR")

    def _open_in_editor(ex: dict) -> None:
        if not edit_cmd:
            return
        src_files = source_files_for(ex, use_solutions)
        if not src_files:
            return
        file_path = str(src_files[0])
        try:
            parts = edit_cmd.split()
            subprocess.Popen([*parts, file_path], cwd=ROOT,
                             stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        except OSError:
            pass

    last_mtime: float = 0.0
    exercise_passed: bool = False
    last_error: str = ""
    show_hint: bool = False
    in_list_mode: bool = False
    list_filter: str = "all"
    needs_render: bool = True

    if os.name != "nt":
        import tty
        import termios
        old_settings = termios.tcgetattr(sys.stdin.fileno())
        tty.setcbreak(sys.stdin.fileno())

    def _restore_terminal() -> None:
        if os.name != "nt":
            import termios as _termios
            _termios.tcsetattr(sys.stdin.fileno(), _termios.TCSADRAIN, old_settings)
        sys.stdout.write("\x1b[?25h")
        sys.stdout.flush()

    def _run_current() -> tuple[bool, str]:
        ex = state.current_exercise()
        try:
            check_one(ex, use_solutions, include_hidden)
            return True, ""
        except Exception as exc:
            return False, str(exc)

    def _check_file_changed() -> bool:
        nonlocal last_mtime
        ex = state.current_exercise()
        src_files = source_files_for(ex, use_solutions)
        current_mtime = get_mtime(src_files)
        if current_mtime > last_mtime:
            last_mtime = current_mtime
            return True
        return False

    def _do_run() -> None:
        nonlocal exercise_passed, last_error, show_hint, needs_render
        passed, error = _run_current()
        exercise_passed = passed
        last_error = error
        show_hint = False
        needs_render = True
        if passed:
            state.mark_done()

    def _render() -> None:
        nonlocal needs_render
        if not needs_render:
            return
        needs_render = False
        ex = state.current_exercise()
        if exercise_passed:
            renderer.render_success(ex)
        else:
            renderer.render_failure(ex, last_error)
        if show_hint:
            renderer.render_hint(ex)

    list_cursor: int = 0
    list_search_query: str = ""
    list_search_active: bool = False

    def _visible_items() -> list[tuple[int, dict, bool]]:
        items = state.exercises_with_status()
        result = []
        for i, (ex, done) in enumerate(items):
            if list_filter == "done" and not done:
                continue
            if list_filter == "pending" and done:
                continue
            if list_search_query and list_search_query.lower() not in ex["name"].lower():
                continue
            result.append((i, ex, done))
        return result

    def _render_list() -> None:
        renderer.render_list(state, list_filter, list_cursor, list_search_query, list_search_active)

    try:
        _open_in_editor(state.current_exercise())
        _check_file_changed()
        _do_run()

        list_needs_render: bool = True

        while True:
            if in_list_mode:
                if list_needs_render:
                    _render_list()
                    list_needs_render = False
                time.sleep(0.1)
                key = read_key_nonblocking()
                if key is None:
                    continue
                list_needs_render = True
                visible = _visible_items()
                n_visible = len(visible)

                if list_search_active:
                    if key == "\x1b" or key == "\r" or key == "\n":
                        list_search_active = False
                    elif key == "\x7f" or key == "\x08":
                        list_search_query = list_search_query[:-1]
                        list_cursor = 0
                    elif key.isprintable() and len(key) == 1:
                        list_search_query += key
                        list_cursor = 0
                    continue

                if key == "q":
                    in_list_mode = False
                    list_search_query = ""
                    list_search_active = False
                    needs_render = True
                elif key == "\x1b":
                    in_list_mode = False
                    list_search_query = ""
                    list_search_active = False
                    needs_render = True
                elif key == "j" or key == "\x1b[B":
                    if n_visible > 0:
                        list_cursor = min(list_cursor + 1, n_visible - 1)
                elif key == "k" or key == "\x1b[A":
                    list_cursor = max(list_cursor - 1, 0)
                elif key == "g":
                    list_cursor = 0
                elif key == "G":
                    list_cursor = max(n_visible - 1, 0)
                elif key == "\r" or key == "\n":
                    if visible:
                        idx, ex_item, _ = visible[list_cursor]
                        state.jump_to(ex_item["name"])
                        in_list_mode = False
                        list_search_query = ""
                        list_search_active = False
                        last_mtime = 0.0
                        _do_run()
                elif key == "d":
                    list_filter = "done"
                    list_cursor = 0
                elif key == "p":
                    list_filter = "pending"
                    list_cursor = 0
                elif key == "a":
                    list_filter = "all"
                    list_cursor = 0
                elif key == "s" or key == "/":
                    list_search_active = True
                    list_search_query = ""
                    list_cursor = 0
                elif key == "r":
                    if visible:
                        _, ex_item, _ = visible[list_cursor]
                        reset_exercise(ex_item)
                        state.mark_pending(ex_item["name"])
                continue

            _render()

            time.sleep(0.1)
            key = read_key_nonblocking()

            if key == "q":
                break
            elif key == "n":
                if exercise_passed:
                    if state.advance_next():
                        _open_in_editor(state.current_exercise())
                        last_mtime = 0.0
                        _do_run()
                        needs_render = True
            elif key == "h":
                show_hint = not show_hint
                needs_render = True
            elif key == "l":
                in_list_mode = True
                list_needs_render = True
                list_filter = "all"
                list_cursor = 0
                list_search_query = ""
                list_search_active = False
            elif key == "r":
                last_mtime = 0.0
                _do_run()
                needs_render = True
            elif key == "x":
                ex = state.current_exercise()
                if reset_exercise(ex):
                    state.mark_pending(ex["name"])
                    last_mtime = 0.0
                    _do_run()
                    needs_render = True
            elif key == "c":
                from ..utils import clear_screen as _cls
                _cls()
                print(f"  {ANSI_BOLD}Checking all exercises...{ANSI_RESET}\n")
                total = state.total
                for i, (ex, _) in enumerate(state.exercises_with_status()):
                    label = f"[{i+1}/{total}]"
                    try:
                        check_one(ex, use_solutions, include_hidden)
                        state._done.add(ex["name"])
                        print(f"  {ANSI_GREEN}{label} \u2714 {ex['name']}{ANSI_RESET}")
                    except Exception as exc:
                        print(f"  {ANSI_RED}{label} \u2718 {ex['name']}{ANSI_RESET}")
                state.save()
                print(f"\n  {progress_bar(state.n_done, state.total)}")
                print(f"\n  {ANSI_DIM}Press any key to return...{ANSI_RESET}", flush=True)
                while read_key_nonblocking() is None:
                    time.sleep(0.05)
                needs_render = True

            if not manual_run and not exercise_passed:
                if _check_file_changed():
                    _do_run()
                    needs_render = True

    except KeyboardInterrupt:
        pass
    finally:
        _restore_terminal()
        print()

    return 0
