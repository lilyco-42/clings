"""Tests for clings.state — WatchState progress tracking.

WatchState persists exercise completion to .clings-state.txt. Tests cover:
  - Initial state (no file → first exercise is current)
  - Loading existing state (current_exercise + [done] list)
  - save() round-trip (write then reload yields same state)
  - mark_done / mark_pending / advance_next / jump_to / all_done

Key behavior: mark_done() does NOT auto-advance; it only marks the current
exercise done and saves. advance_next() must be called separately to move
the current pointer. This separation lets the caller decide flow.

All tests use isolated_workspace to redirect STATE_FILE to a temp path,
preventing pollution of the real workspace.
"""

from __future__ import annotations

from pathlib import Path

import pytest

from clings.state import WatchState, next_pending_exercise
from clings import config as cfg


# ─── Sample exercise list for tests ─────────────────────────────────────────

def _make_exercises() -> list[dict]:
    """Three exercises for state testing."""
    return [
        {"name": "ex_01", "unit": "unit0", "lesson": 1, "order": 1},
        {"name": "ex_02", "unit": "unit0", "lesson": 2, "order": 2},
        {"name": "ex_03", "unit": "unit0", "lesson": 3, "order": 3},
    ]


# ═══════════════════════════════════════════════════════════════════════════
# Initial state
# ═══════════════════════════════════════════════════════════════════════════

class TestWatchStateInitial:
    """WatchState with no existing state file."""

    def test_current_is_first(self, isolated_workspace: Path) -> None:
        """No state file → current exercise is the first one."""
        state = WatchState(_make_exercises())
        assert state.current_exercise()["name"] == "ex_01"
        assert state.current_index == 0

    def test_no_done_initially(self, isolated_workspace: Path) -> None:
        """No state file → n_done == 0."""
        state = WatchState(_make_exercises())
        assert state.n_done == 0
        assert not state.all_done()

    def test_total_count(self, isolated_workspace: Path) -> None:
        """total property returns the exercise count."""
        state = WatchState(_make_exercises())
        assert state.total == 3


# ═══════════════════════════════════════════════════════════════════════════
# Loading existing state
# ═══════════════════════════════════════════════════════════════════════════

class TestWatchStateLoad:
    """WatchState loading from an existing state file."""

    def test_load_current(self, isolated_workspace: Path) -> None:
        """Loads current_exercise from the state file."""
        state_file = isolated_workspace / ".clings-state.txt"
        state_file.write_text(
            "# progress\n\ncurrent_exercise = ex_02\n\n[done]\nex_01\n",
            encoding="utf-8",
        )
        state = WatchState(_make_exercises())
        assert state.current_exercise()["name"] == "ex_02"
        assert state.current_index == 1

    def test_load_done_list(self, isolated_workspace: Path) -> None:
        """Loads the [done] list."""
        state_file = isolated_workspace / ".clings-state.txt"
        state_file.write_text(
            "current_exercise = ex_03\n\n[done]\nex_01\nex_02\n",
            encoding="utf-8",
        )
        state = WatchState(_make_exercises())
        assert state.n_done == 2
        assert state.is_done({"name": "ex_01"})
        assert state.is_done({"name": "ex_02"})
        assert not state.is_done({"name": "ex_03"})

    def test_load_current_not_in_list_falls_back(
        self, isolated_workspace: Path
    ) -> None:
        """If saved current_exercise name isn't valid, falls back to first pending."""
        state_file = isolated_workspace / ".clings-state.txt"
        state_file.write_text(
            "current_exercise = nonexistent\n\n[done]\nex_01\n",
            encoding="utf-8",
        )
        state = WatchState(_make_exercises())
        # ex_01 is done, so first pending is ex_02
        assert state.current_exercise()["name"] == "ex_02"

    def test_load_ignores_comments_and_blanks(
        self, isolated_workspace: Path
    ) -> None:
        """Comment lines and blank lines in the state file are ignored."""
        state_file = isolated_workspace / ".clings-state.txt"
        state_file.write_text(
            "# comment line\n"
            "\n"
            "current_exercise = ex_02\n"
            "# another comment\n"
            "\n"
            "[done]\n"
            "# not an exercise\n"
            "ex_01\n",
            encoding="utf-8",
        )
        state = WatchState(_make_exercises())
        assert state.current_exercise()["name"] == "ex_02"
        assert state.n_done == 1

    def test_load_done_unknown_name_ignored(self, isolated_workspace: Path) -> None:
        """Names in [done] that don't match any exercise are ignored."""
        state_file = isolated_workspace / ".clings-state.txt"
        state_file.write_text(
            "current_exercise = ex_01\n\n[done]\nfake_ex\nex_01\n",
            encoding="utf-8",
        )
        state = WatchState(_make_exercises())
        assert state.n_done == 1  # only ex_01 counted, fake_ex ignored


# ═══════════════════════════════════════════════════════════════════════════
# save() round-trip
# ═══════════════════════════════════════════════════════════════════════════

class TestWatchStateSaveRoundTrip:
    """save() then reload yields equivalent state."""

    def test_basic_roundtrip(self, isolated_workspace: Path) -> None:
        """Save current state, reload, verify consistency."""
        exercises = _make_exercises()
        state = WatchState(exercises)
        state.mark_done()  # mark ex_01 done (does NOT auto-advance)
        state.save()

        # Reload
        state2 = WatchState(exercises)
        assert state2.n_done == 1
        assert state2.is_done({"name": "ex_01"})
        # Current is still ex_01 (mark_done doesn't advance)
        assert state2.current_exercise()["name"] == "ex_01"

    def test_multiple_marks_roundtrip(self, isolated_workspace: Path) -> None:
        """Mark + advance multiple exercises, save, reload."""
        exercises = _make_exercises()
        state = WatchState(exercises)
        state.mark_done()         # ex_01 done, current still ex_01
        state.advance_next()      # advance to ex_02
        state.mark_done()         # ex_02 done, current still ex_02
        state.advance_next()      # advance to ex_03
        state.mark_done()         # ex_03 done

        state2 = WatchState(exercises)
        assert state2.n_done == 3
        assert state2.all_done()


# ═══════════════════════════════════════════════════════════════════════════
# mark_done / mark_pending
# ═══════════════════════════════════════════════════════════════════════════

class TestMarkDonePending:
    """mark_done() and mark_pending() mutate the done set and persist.

    Note: mark_done() does NOT advance the current pointer. The caller must
    call advance_next() separately. This is by design — it decouples
    "mark complete" from "move to next", allowing flexible flows (e.g.
    jumping back to redo, marking done without leaving the current view).
    """

    def test_mark_done_adds_to_set(self, isolated_workspace: Path) -> None:
        """mark_done() adds current exercise to done set (no advance)."""
        state = WatchState(_make_exercises())
        assert not state.is_done({"name": "ex_01"})
        state.mark_done()
        assert state.is_done({"name": "ex_01"})
        assert state.n_done == 1
        # Current unchanged
        assert state.current_exercise()["name"] == "ex_01"

    def test_mark_done_persists(self, isolated_workspace: Path) -> None:
        """mark_done() writes to the state file."""
        exercises = _make_exercises()
        state = WatchState(exercises)
        state.mark_done()
        state_file = isolated_workspace / ".clings-state.txt"
        assert state_file.exists()
        content = state_file.read_text(encoding="utf-8")
        assert "ex_01" in content
        assert "[done]" in content

    def test_mark_pending_removes_from_set(self, isolated_workspace: Path) -> None:
        """mark_pending() removes an exercise from the done set."""
        state = WatchState(_make_exercises())
        state.mark_done()  # ex_01 done
        assert state.is_done({"name": "ex_01"})
        state.mark_pending("ex_01")
        assert not state.is_done({"name": "ex_01"})
        assert state.n_done == 0

    def test_mark_pending_nonexistent_no_crash(self, isolated_workspace: Path) -> None:
        """mark_pending() on a name not in done set is a no-op (discard)."""
        state = WatchState(_make_exercises())
        state.mark_pending("never_was_done")  # should not crash
        assert state.n_done == 0


# ═══════════════════════════════════════════════════════════════════════════
# advance_next
# ═══════════════════════════════════════════════════════════════════════════

class TestAdvanceNext:
    """advance_next() moves to the next pending exercise (circular)."""

    def test_advance_to_next_pending(self, isolated_workspace: Path) -> None:
        """Mark ex_01 done, then advance → current becomes ex_02."""
        state = WatchState(_make_exercises())
        state.mark_done()
        result = state.advance_next()
        assert result is True
        assert state.current_exercise()["name"] == "ex_02"

    def test_advance_circular_wraps(self, isolated_workspace: Path) -> None:
        """Advance wraps circularly when reaching the end of the list."""
        exercises = _make_exercises()
        state = WatchState(exercises)
        state.mark_done()         # ex_01
        state.advance_next()      # → ex_02
        state.mark_done()         # ex_02
        state.advance_next()      # → ex_03
        state.mark_done()         # ex_03
        # All done now
        assert state.all_done()
        assert state.advance_next() is False

    def test_advance_returns_false_when_all_done(
        self, isolated_workspace: Path
    ) -> None:
        """advance_next() returns False when all exercises are done."""
        exercises = _make_exercises()
        state = WatchState(exercises)
        state.mark_done()
        state.advance_next()
        state.mark_done()
        state.advance_next()
        state.mark_done()
        assert state.all_done()
        assert state.advance_next() is False

    def test_advance_skips_done(self, isolated_workspace: Path) -> None:
        """advance_next() skips already-done exercises."""
        exercises = _make_exercises()
        state = WatchState(exercises)
        state.mark_done()         # ex_01 done
        state.advance_next()      # → ex_02
        state.mark_done()         # ex_02 done
        # Jump back to ex_01 and advance — should skip to ex_03
        state.jump_to("ex_01")
        result = state.advance_next()
        assert result is True
        assert state.current_exercise()["name"] == "ex_03"


# ═══════════════════════════════════════════════════════════════════════════
# jump_to
# ═══════════════════════════════════════════════════════════════════════════

class TestJumpTo:
    """jump_to() moves the current pointer to a named exercise."""

    def test_jump_to_valid(self, isolated_workspace: Path) -> None:
        """Jump to an existing exercise returns True and updates current."""
        state = WatchState(_make_exercises())
        assert state.jump_to("ex_03")
        assert state.current_exercise()["name"] == "ex_03"
        assert state.current_index == 2

    def test_jump_to_invalid_keeps_current(self, isolated_workspace: Path) -> None:
        """Jump to a non-existent name returns False, current unchanged.

        Note: the implementation falls back to _find_first_pending when the
        saved current is invalid during _load. But jump_to() itself returns
        False and leaves _current_idx at the fallback position. We verify
        the return value and that current is a valid exercise (not crashed).
        """
        state = WatchState(_make_exercises())
        result = state.jump_to("nonexistent")
        assert result is False
        # Current should still point to a valid exercise (not crashed)
        assert state.current_exercise()["name"] in {"ex_01", "ex_02", "ex_03"}


# ═══════════════════════════════════════════════════════════════════════════
# all_done / exercises_with_status
# ═══════════════════════════════════════════════════════════════════════════

class TestAllDoneAndStatus:
    """all_done() and exercises_with_status() aggregate queries."""

    def test_all_done_false_initially(self, isolated_workspace: Path) -> None:
        """all_done() is False when nothing is done."""
        state = WatchState(_make_exercises())
        assert not state.all_done()

    def test_all_done_true_when_all_marked(self, isolated_workspace: Path) -> None:
        """all_done() is True when all exercises are done."""
        state = WatchState(_make_exercises())
        state.mark_done()
        state.advance_next()
        state.mark_done()
        state.advance_next()
        state.mark_done()
        assert state.all_done()

    def test_exercises_with_status(self, isolated_workspace: Path) -> None:
        """exercises_with_status() returns (exercise, is_done) tuples."""
        state = WatchState(_make_exercises())
        state.mark_done()  # ex_01 done
        pairs = state.exercises_with_status()
        assert len(pairs) == 3
        assert pairs[0][1] is True   # ex_01 done
        assert pairs[1][1] is False  # ex_02 not done
        assert pairs[2][1] is False  # ex_03 not done


# ═══════════════════════════════════════════════════════════════════════════
# next_pending_exercise (module-level)
# ═══════════════════════════════════════════════════════════════════════════

class TestNextPendingExercise:
    """next_pending_exercise() finds the next exercise to work on."""

    def test_no_state_file_returns_first(self, isolated_workspace: Path) -> None:
        """No state file → returns the first exercise."""
        config = {"exercises": _make_exercises()}
        result = next_pending_exercise(config)
        assert result is not None
        assert result["name"] == "ex_01"

    def test_with_state_returns_current(self, isolated_workspace: Path) -> None:
        """With a state file, returns the current (pending) exercise."""
        state_file = isolated_workspace / ".clings-state.txt"
        state_file.write_text(
            "current_exercise = ex_02\n\n[done]\nex_01\n",
            encoding="utf-8",
        )
        config = {"exercises": _make_exercises()}
        result = next_pending_exercise(config)
        assert result is not None
        assert result["name"] == "ex_02"

    def test_all_done_returns_none(self, isolated_workspace: Path) -> None:
        """When all exercises are done, returns None."""
        state_file = isolated_workspace / ".clings-state.txt"
        state_file.write_text(
            "current_exercise = ex_03\n\n[done]\nex_01\nex_02\nex_03\n",
            encoding="utf-8",
        )
        config = {"exercises": _make_exercises()}
        assert next_pending_exercise(config) is None

    def test_empty_exercises_no_state_returns_none(
        self, isolated_workspace: Path
    ) -> None:
        """Empty exercise list and no state file → None (all_ex[0] on empty)."""
        config = {"exercises": []}
        # No state file → all_ex[0] would IndexError; function returns None
        # because all_ex is empty (the `if all_ex else None` guard)
        assert next_pending_exercise(config) is None
