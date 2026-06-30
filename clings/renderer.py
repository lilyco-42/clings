"""Watch mode UI renderer."""

from .config import ROOT
from .state import WatchState
from .utils import (
    ANSI_BOLD,
    ANSI_BOLD_CYAN,
    ANSI_BOLD_GREEN,
    ANSI_BOLD_RED,
    ANSI_CYAN,
    ANSI_DIM,
    ANSI_GREEN,
    ANSI_RED,
    ANSI_RESET,
    ANSI_YELLOW,
    clear_screen,
    progress_bar,
    source_files_for,
    terminal_hyperlink,
)


class WatchRenderer:
    """Renders the watch mode UI (success, failure, hint, list views)."""

    def __init__(self, state: WatchState) -> None:
        self._state = state
        self.auto_advance: bool = False

    def render_success(self, ex: dict) -> None:
        clear_screen()
        s = self._state
        print(f"{ANSI_BOLD_GREEN}{progress_bar(s.n_done, s.total)}{ANSI_RESET}")
        print()
        src_files = source_files_for(ex, False)
        rel_path = src_files[0].relative_to(ROOT) if src_files else ex["name"]
        link = terminal_hyperlink(src_files[0], str(rel_path)) if src_files else str(rel_path)
        print(f"  {ANSI_BOLD_GREEN}\u2705 Exercise done!{ANSI_RESET}  {ex['name']}")
        print(f"  {ANSI_DIM}File: {link}{ANSI_RESET}")
        if s.all_done():
            print(f"\n  {ANSI_BOLD_GREEN}\U0001f389 Congratulations! All {s.total} exercises completed!{ANSI_RESET}")
            print(f"\n  {ANSI_DIM}Press q to quit.{ANSI_RESET}")
        else:
            mode_tag = "  (auto-advance: on)" if self.auto_advance else ""
            print(f"\n  {ANSI_DIM}Commands: n:next  h:hint  t:tests  l:list  c:check  x:reset  q:quit{mode_tag}{ANSI_RESET}")

    def render_failure(self, ex: dict, error: str) -> None:
        clear_screen()
        s = self._state
        print(f"{ANSI_YELLOW}{progress_bar(s.n_done, s.total)}{ANSI_RESET}")
        print()
        src_files = source_files_for(ex, False)
        rel_path = src_files[0].relative_to(ROOT) if src_files else ex["name"]
        link = terminal_hyperlink(src_files[0], str(rel_path)) if src_files else str(rel_path)
        print(f"  {ANSI_BOLD_RED}\u274c Current: {ex['name']}{ANSI_RESET}")
        print(f"  {ANSI_DIM}File: {link}{ANSI_RESET}")
        print(f"  {ANSI_DIM}Title: {ex['title']}{ANSI_RESET}")
        print()
        for line in error.splitlines()[:30]:
            print(f"  {line}")
        if len(error.splitlines()) > 30:
            print(f"  {ANSI_DIM}... (output truncated){ANSI_RESET}")
        mode_tag = "  (auto-advance: on)" if self.auto_advance else ""
        print(f"\n  {ANSI_DIM}Commands: h:hint  t:tests  l:list  c:check  r:rerun  x:reset  q:quit{mode_tag}{ANSI_RESET}")

    def render_hint(self, ex: dict) -> None:
        hint = ex.get("hint", "No hint available for this exercise.")
        print(f"\n  {ANSI_BOLD_CYAN}\U0001f4a1 Hint:{ANSI_RESET}")
        for line in hint.splitlines():
            print(f"  {ANSI_CYAN}{line}{ANSI_RESET}")
        print()

    def render_list(self, state: WatchState, filter_mode: str = "all",
                    cursor: int = 0, search_query: str = "",
                    search_active: bool = False) -> None:
        clear_screen()
        mode_label = f"search: {search_query}" if search_active else f"filter: {filter_mode}"
        print(f"{ANSI_BOLD}  Exercise List{ANSI_RESET}  "
              f"{ANSI_DIM}({mode_label}){ANSI_RESET}")
        print(f"  {progress_bar(state.n_done, state.total)}")
        print(f"  {ANSI_DIM}j/k:\u2191\u2193  d:done p:pending a:all  "
              f"s:search  r:reset  Enter:jump  q:back{ANSI_RESET}")
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
            marker = f"{ANSI_GREEN}\u2714{ANSI_RESET}" if done else f"{ANSI_RED}\u2022{ANSI_RESET}"
            is_cursor = vi == cursor
            prefix = f"{ANSI_BOLD_CYAN}\u25b6{ANSI_RESET}" if is_cursor else " "
            name = ex["name"]
            title = ex["title"]
            lesson = f"L{ex['lesson']:02d}"
            if is_cursor:
                print(f"  {prefix} {marker} {lesson} {ANSI_BOLD}{name:32s}{ANSI_RESET} {ANSI_DIM}{title}{ANSI_RESET}")
            else:
                print(f"  {prefix} {marker} {lesson} {name:32s} {ANSI_DIM}{title}{ANSI_RESET}")
        if not visible:
            print(f"  {ANSI_DIM}(no exercises match){ANSI_RESET}")
        if search_active:
            print(f"\n  {ANSI_BOLD_CYAN}/{search_query}\u2588{ANSI_RESET}", end="", flush=True)
