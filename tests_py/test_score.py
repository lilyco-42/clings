"""Tests for clings.commands.score._detect_status.

_detect_status() distinguishes NOT_COMPLETED (student hasn't started —
source still has the "I AM NOT DONE" marker) from FAILED (student tried
but the code doesn't pass). This drives the score report's status field
in CI grading.
"""

from __future__ import annotations

from pathlib import Path

import pytest

from clings.commands.score import _detect_status


class TestDetectStatus:
    """_detect_status() inspects source files for the NOT_DONE marker."""

    def test_not_done_marker_returns_not_completed(
        self, isolated_workspace: Path
    ) -> None:
        """Source containing 'I AM NOT DONE' → NOT_COMPLETED."""
        ex_dir = isolated_workspace / "exercises" / "todo_ex"
        ex_dir.mkdir(parents=True)
        src = ex_dir / "main.c"
        src.write_text(
            "int main(void){\n"
            "  /* I AM NOT DONE */\n"
            "  return 0;\n"
            "}\n",
            encoding="utf-8",
        )
        ex = {"name": "todo_ex", "path": "todo_ex", "source": "main.c"}
        assert _detect_status(ex, use_solutions=False, error_msg="some error") == "NOT_COMPLETED"

    def test_no_marker_returns_failed(self, isolated_workspace: Path) -> None:
        """Source without the marker → FAILED (student attempted it)."""
        ex_dir = isolated_workspace / "exercises" / "attempted_ex"
        ex_dir.mkdir(parents=True)
        src = ex_dir / "main.c"
        src.write_text("int main(void){return 1;}\n", encoding="utf-8")
        ex = {"name": "attempted_ex", "path": "attempted_ex", "source": "main.c"}
        assert _detect_status(ex, use_solutions=False, error_msg="mismatch") == "FAILED"

    def test_missing_source_file(self) -> None:
        """Non-existent source file → FAILED (can't prove NOT_COMPLETED)."""
        ex = {"name": "ghost", "path": "ghost", "source": "ghost.c"}
        assert _detect_status(ex, use_solutions=False, error_msg="") == "FAILED"

    def test_missing_source_dir(self) -> None:
        """Non-existent source directory → FAILED."""
        ex = {"name": "ghostdir", "path": "ghostdir", "source": "main.c"}
        assert _detect_status(ex, use_solutions=False, error_msg="") == "FAILED"

    def test_multiple_sources_one_not_done(self, isolated_workspace: Path) -> None:
        """If ANY source file has the marker → NOT_COMPLETED."""
        ex_dir = isolated_workspace / "exercises" / "multi_src"
        ex_dir.mkdir(parents=True)
        (ex_dir / "a.c").write_text("int a(void){return 0;}\n", encoding="utf-8")
        (ex_dir / "b.c").write_text("/* I AM NOT DONE */\n", encoding="utf-8")
        ex = {"name": "multi_src", "path": "multi_src",
              "sources": ["a.c", "b.c"]}
        assert _detect_status(ex, use_solutions=False, error_msg="") == "NOT_COMPLETED"

    def test_empty_source_file(self, isolated_workspace: Path) -> None:
        """Empty source file (no marker) → FAILED."""
        ex_dir = isolated_workspace / "exercises" / "empty_ex"
        ex_dir.mkdir(parents=True)
        (ex_dir / "main.c").write_text("", encoding="utf-8")
        ex = {"name": "empty_ex", "path": "empty_ex", "source": "main.c"}
        assert _detect_status(ex, use_solutions=False, error_msg="") == "FAILED"

    def test_marker_anywhere_in_file(self, isolated_workspace: Path) -> None:
        """The marker can appear anywhere in the file (not just comments)."""
        ex_dir = isolated_workspace / "exercises" / "marker_pos"
        ex_dir.mkdir(parents=True)
        (ex_dir / "main.c").write_text(
            "#define MARKER I AM NOT DONE\nint main(void){return 0;}\n",
            encoding="utf-8",
        )
        ex = {"name": "marker_pos", "path": "marker_pos", "source": "main.c"}
        assert _detect_status(ex, use_solutions=False, error_msg="") == "NOT_COMPLETED"
