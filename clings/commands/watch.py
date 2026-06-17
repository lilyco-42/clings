"""clings watch — interactive watch mode with file change detection."""

import argparse
import os
import subprocess
import sys
import time

from ..compiler import check_one
from ..config import (
    ROOT,
    STATE_FILE,
    exercises,
    load_config,
    select_exercises,
    source_dir_for,
)


# ─── ANSI Constants ──────────────────────────────────────────────────────────

_ANSI_RESET = "\x1b[0m"
_ANSI_BOLD = "\x1b[1m"
_ANSI_DIM = "\x1b[2m"
_ANSI_RED = "\x1b[31m"
_ANSI_GREEN = "\x1b[32m"
_ANSI_YELLOW = "\x1b[33m"
_ANSI_BLUE = "\x1b[34m"
_ANSI_CYAN = "\x1b[36m"
_ANSI_BOLD_RED = "\x1b[31;1m"
_ANSI_BOLD_GREEN = "\x1b[32;1m"
_ANSI_BOLD_CYAN = "\x1b[36;1m"


# ─── Terminal Helpers ─────────────────────────────────────────────────────────

def _clear_screen() -> None:
    sys.stdout.write("\x1b[2J\x1b[H")
    sys.stdout.flush()


def _terminal_width() -> int:
    try:
        return os.get_terminal_size().columns
    except OSError:
        return 80


def _progress_bar(n_done: int, total: int, width: int | None = None) -> str:
    if width is None:
        width = min(_terminal_width() - 30, 40)
    width = max(width, 10)
    if total == 0:
        return ""
    filled = int(width * n_done / total)
    bar = "\u2588" * filled + "\u2591" * (width - filled)
    pct = 100.0 * n_done / total
    return f"Progress: [{bar}] {n_done}/{total} ({pct:.1f}%)"


def _terminal_hyperlink(path: "os.PathLike[str]", display: str | None = None) -> str:
    from pathlib import Path
    abs_path = Path(path).resolve()
    if display is None:
        display = str(path)
    uri = abs_path.as_uri()
    return f"\x1b]8;;{uri}\x1b\\{display}\x1b]8;;\x1b\\"


def _source_files_for(ex: dict, use_solutions: bool) -> list:
    from pathlib import Path
    src_dir = source_dir_for(ex, use_solutions)
    if not src_dir.exists():
        return []
    if "sources" in ex:
        return [src_dir / s for s in ex["sources"]]
    elif "source" in ex:
        return [src_dir / ex["source"]]
    else:
        return sorted(src_dir.glob("*.c"))


def _get_mtime(files: list) -> float:
    mtime = 0.0
    for f in files:
        try:
            mtime = max(mtime, f.stat().st_mtime)
        except OSError:
            pass
    return mtime


def _read_key_nonblocking() -> str | None:
    if os.name == "nt":
        import msvcrt
        if msvcrt.kbhit():
            ch = msvcrt.getwch()
            return ch
        return None
    else:
        import select as _select
        rlist, _, _ = _select.select([sys.stdin], [], [], 0)
        if rlist:
            ch = sys.stdin.read(1)
            return ch
        return None


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


# ─── State Management ────────────────────────────────────────────────────────

class WatchState:
    def __init__(self, exercises: list[dict]) -> None:
        self._exercises = exercises
        self._name_to_idx: dict[str, int] = {
            ex["name"]: i for i, ex in enumerate(exercises)
        }
        self._done: set[str] = set()
        self._current_idx: int = 0
        self._load()

    def _load(self) -> None:
        if not STATE_FILE.exists():
            return
        try:
            text = STATE_FILE.read_text(encoding="utf-8")
        except OSError:
            return
        current_name: str | None = None
        in_done = False
        for line in text.splitlines():
            stripped = line.strip()
            if stripped.startswith("#") or not stripped:
                continue
            if stripped == "[done]":
                in_done = True
                continue
            if not in_done:
                if stripped.startswith("current_exercise"):
                    _, _, val = stripped.partition("=")
                    current_name = val.strip()
            else:
                if stripped in self._name_to_idx:
                    self._done.add(stripped)
        if current_name and current_name in self._name_to_idx:
            self._current_idx = self._name_to_idx[current_name]
        else:
            self._find_first_pending()

    def _find_first_pending(self) -> None:
        for i, ex in enumerate(self._exercises):
            if ex["name"] not in self._done:
                self._current_idx = i
                return
        self._current_idx = len(self._exercises) - 1

    def save(self) -> None:
        lines = [
            "# Clings progress file. Remove this file to reset all progress.\n",
            "\n",
            f"current_exercise = {self.current_exercise()['name']}\n",
            "\n",
            "[done]\n",
        ]
        for ex in self._exercises:
            if ex["name"] in self._done:
                lines.append(f"{ex['name']}\n")
        try:
            STATE_FILE.write_text("".join(lines), encoding="utf-8")
        except OSError:
            pass

    def current_exercise(self) -> dict:
        return self._exercises[self._current_idx]

    @property
    def current_index(self) -> int:
        return self._current_idx

    @property
    def total(self) -> int:
        return len(self._exercises)

    @property
    def n_done(self) -> int:
        return len(self._done)

    def is_done(self, ex: dict) -> bool:
        return ex["name"] in self._done

    def mark_done(self) -> None:
        self._done.add(self.current_exercise()["name"])
        self.save()

    def mark_pending(self, name: str) -> None:
        self._done.discard(name)
        self.save()

    def advance_next(self) -> bool:
        start = self._current_idx
        for offset in range(1, self.total):
            idx = (start + offset) % self.total
            if self._exercises[idx]["name"] not in self._done:
                self._current_idx = idx
                self.save()
                return True
        return False

    def jump_to(self, name: str) -> bool:
        if name in self._name_to_idx:
            self._current_idx = self._name_to_idx[name]
            self.save()
            return True
        return False

    def all_done(self) -> bool:
        return len(self._done) >= self.total

    def exercises_with_status(self) -> list[tuple[dict, bool]]:
        return [(ex, ex["name"] in self._done) for ex in self._exercises]


# ─── Renderer ────────────────────────────────────────────────────────────────

class WatchRenderer:
    def __init__(self, state: WatchState) -> None:
        self._state = state

    def render_success(self, ex: dict) -> None:
        _clear_screen()
        s = self._state
        print(f"{_ANSI_BOLD_GREEN}{_progress_bar(s.n_done, s.total)}{_ANSI_RESET}")
        print()
        src_files = _source_files_for(ex, False)
        rel_path = src_files[0].relative_to(ROOT) if src_files else ex["name"]
        link = _terminal_hyperlink(src_files[0], str(rel_path)) if src_files else str(rel_path)
        print(f"  {_ANSI_BOLD_GREEN}\u2705 Exercise done!{_ANSI_RESET}  {ex['name']}")
        print(f"  {_ANSI_DIM}File: {link}{_ANSI_RESET}")
        if s.all_done():
            print(f"\n  {_ANSI_BOLD_GREEN}\U0001f389 Congratulations! All {s.total} exercises completed!{_ANSI_RESET}")
            print(f"\n  {_ANSI_DIM}Press q to quit.{_ANSI_RESET}")
        else:
            print(f"\n  {_ANSI_DIM}Commands: n:next  h:hint  l:list  x:reset  q:quit{_ANSI_RESET}")

    def render_failure(self, ex: dict, error: str) -> None:
        _clear_screen()
        s = self._state
        print(f"{_ANSI_YELLOW}{_progress_bar(s.n_done, s.total)}{_ANSI_RESET}")
        print()
        src_files = _source_files_for(ex, False)
        rel_path = src_files[0].relative_to(ROOT) if src_files else ex["name"]
        link = _terminal_hyperlink(src_files[0], str(rel_path)) if src_files else str(rel_path)
        print(f"  {_ANSI_BOLD_RED}\u274c Current: {ex['name']}{_ANSI_RESET}")
        print(f"  {_ANSI_DIM}File: {link}{_ANSI_RESET}")
        print(f"  {_ANSI_DIM}Title: {ex['title']}{_ANSI_RESET}")
        print()
        for line in error.splitlines()[:30]:
            print(f"  {line}")
        if len(error.splitlines()) > 30:
            print(f"  {_ANSI_DIM}... (output truncated){_ANSI_RESET}")
        print(f"\n  {_ANSI_DIM}Commands: h:hint  l:list  r:rerun  x:reset  q:quit{_ANSI_RESET}")

    def render_hint(self, ex: dict) -> None:
        hint = ex.get("hint", "No hint available for this exercise.")
        print(f"\n  {_ANSI_BOLD_CYAN}\U0001f4a1 Hint:{_ANSI_RESET}")
        for line in hint.splitlines():
            print(f"  {_ANSI_CYAN}{line}{_ANSI_RESET}")
        print()

    def render_list(self, state: WatchState, filter_mode: str = "all",
                    cursor: int = 0, search_query: str = "",
                    search_active: bool = False) -> None:
        _clear_screen()
        mode_label = f"search: {search_query}" if search_active else f"filter: {filter_mode}"
        print(f"{_ANSI_BOLD}  Exercise List{_ANSI_RESET}  "
              f"{_ANSI_DIM}({mode_label}){_ANSI_RESET}")
        print(f"  {_progress_bar(state.n_done, state.total)}")
        print(f"  {_ANSI_DIM}j/k:\u2191\u2193  d:done p:pending a:all  "
              f"s:search  r:reset  Enter:jump  q:back{_ANSI_RESET}")
        print()
        items = state.exercises_with_status()
        visible: list[tuple[int, dict, bool]] = []
        for i, (ex, done) in enumerate(items):
            if filter_mode == "done" and not done:
                continue
            if filter_mode == "pending" and done:
                continue
            if search_query and search_query.lower() not in ex["name"].lower():
                continue
            visible.append((i, ex, done))
        for vi, (i, ex, done) in enumerate(visible):
            marker = f"{_ANSI_GREEN}\u2714{_ANSI_RESET}" if done else f"{_ANSI_RED}\u2022{_ANSI_RESET}"
            is_cursor = vi == cursor
            prefix = f"{_ANSI_BOLD_CYAN}\u25b6{_ANSI_RESET}" if is_cursor else " "
            name = ex["name"]
            title = ex["title"]
            lesson = f"L{ex['lesson']:02d}"
            if is_cursor:
                print(f"  {prefix} {marker} {lesson} {_ANSI_BOLD}{name:32s}{_ANSI_RESET} {_ANSI_DIM}{title}{_ANSI_RESET}")
            else:
                print(f"  {prefix} {marker} {lesson} {name:32s} {_ANSI_DIM}{title}{_ANSI_RESET}")
        if not visible:
            print(f"  {_ANSI_DIM}(no exercises match){_ANSI_RESET}")
        if search_active:
            print(f"\n  {_ANSI_BOLD_CYAN}/{search_query}\u2588{_ANSI_RESET}", end="", flush=True)


# ─── Next Pending Helper ─────────────────────────────────────────────────────

def _next_pending_exercise(config: dict) -> dict | None:
    all_ex = exercises(config)
    if not STATE_FILE.exists():
        return all_ex[0] if all_ex else None
    state = WatchState(all_ex)
    ex = state.current_exercise()
    return None if state.is_done(ex) and state.all_done() else ex


# ─── Main Watch Loop ─────────────────────────────────────────────────────────

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
        src_files = _source_files_for(ex, use_solutions)
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
        src_files = _source_files_for(ex, use_solutions)
        current_mtime = _get_mtime(src_files)
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
                key = _read_key_nonblocking()
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
                        _reset_exercise(ex_item)
                        state.mark_pending(ex_item["name"])
                continue

            _render()

            time.sleep(0.1)
            key = _read_key_nonblocking()

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
                if _reset_exercise(ex):
                    state.mark_pending(ex["name"])
                    last_mtime = 0.0
                    _do_run()
                    needs_render = True
            elif key == "c":
                _clear_screen()
                print(f"  {_ANSI_BOLD}Checking all exercises...{_ANSI_RESET}\n")
                total = state.total
                for i, (ex, _) in enumerate(state.exercises_with_status()):
                    label = f"[{i+1}/{total}]"
                    try:
                        check_one(ex, use_solutions, include_hidden)
                        state._done.add(ex["name"])
                        print(f"  {_ANSI_GREEN}{label} \u2714 {ex['name']}{_ANSI_RESET}")
                    except Exception as exc:
                        print(f"  {_ANSI_RED}{label} \u2718 {ex['name']}{_ANSI_RESET}")
                state.save()
                print(f"\n  {_progress_bar(state.n_done, state.total)}")
                print(f"\n  {_ANSI_DIM}Press any key to return...{_ANSI_RESET}", flush=True)
                while _read_key_nonblocking() is None:
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
