"""Exercise progress state management."""

from .config import STATE_FILE, exercises


class WatchState:
    """Tracks exercise completion progress and current position."""

    def __init__(self, exercises: list[dict]) -> None:
        self._exercises = exercises
        self._name_to_idx: dict[str, int] = {
            ex["name"]: i for i, ex in enumerate(exercises)
        }
        self._done: set[str] = set()
        # Done-records for exercises NOT in `exercises` (this state's, possibly
        # filtered, list). Preserved verbatim on save so a subset WatchState
        # (e.g. `clings watch unit1`) never clobbers other units' progress.
        self._extra_done: set[str] = set()
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
                else:
                    self._extra_done.add(stripped)
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
        # Write back done-records for exercises outside this state's list so a
        # filtered WatchState never erases progress it wasn't tracking.
        for name in sorted(self._extra_done):
            lines.append(f"{name}\n")
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

    @property
    def done_names(self) -> frozenset[str]:
        """Names in this state's exercise list that are marked done (read-only)."""
        return frozenset(self._done)

    def set_done(self, name: str, done: bool) -> None:
        """Mark ``name`` done or pending without moving the current pointer.

        Only affects exercises in this state's list; unknown names are ignored
        (their records, if any, live in ``_extra_done`` and are untouched).
        Does not persist — call :meth:`save` when a batch of updates is done.
        """
        if done:
            if name in self._name_to_idx:
                self._done.add(name)
        else:
            self._done.discard(name)

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


def next_pending_exercise(config: dict) -> dict | None:
    """Find the next pending exercise, or None if all completed."""
    all_ex = exercises(config)
    if not STATE_FILE.exists():
        return all_ex[0] if all_ex else None
    state = WatchState(all_ex)
    ex = state.current_exercise()
    return None if state.is_done(ex) and state.all_done() else ex
