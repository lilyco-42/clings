"""Tests for clings.utils — terminal helpers and file utilities.

Covers the pure-function layer: progress_bar (with edge cases), get_mtime,
terminal_hyperlink (OSC 8 format). Terminal-IO functions (clear_screen,
read_key_nonblocking, terminal_width) are environment-dependent and
excluded from this layer (known gap).
"""

from __future__ import annotations

import time
from pathlib import Path

import pytest

from clings.utils import (
    progress_bar,
    get_mtime,
    terminal_hyperlink,
    source_files_for,
    watch_files_for,
)


# ─── progress_bar() ─────────────────────────────────────────────────────────

class TestProgressBar:
    """progress_bar() renders a text progress bar with percentage."""

    def test_basic_half(self) -> None:
        """50% progress renders half-filled bar with 50.0%."""
        result = progress_bar(5, 10, width=20)
        assert "5/10" in result
        assert "50.0%" in result
        # 20 width, 5/10 → 10 filled, 10 empty
        assert result.count("\u2588") == 10  # filled blocks
        assert result.count("\u2591") == 10  # empty blocks

    def test_zero_total(self) -> None:
        """total=0 returns empty string (avoids division by zero)."""
        assert progress_bar(0, 0, width=20) == ""
        assert progress_bar(5, 0, width=20) == ""

    def test_full(self) -> None:
        """n_done == total → fully filled bar, 100.0%."""
        result = progress_bar(10, 10, width=20)
        assert "10/10" in result
        assert "100.0%" in result
        assert result.count("\u2588") == 20
        assert result.count("\u2591") == 0

    def test_zero_done(self) -> None:
        """n_done=0 → empty bar, 0.0%."""
        result = progress_bar(0, 10, width=20)
        assert "0/10" in result
        assert "0.0%" in result
        assert result.count("\u2588") == 0
        assert result.count("\u2591") == 20

    def test_min_width_enforced(self) -> None:
        """Width is clamped to minimum 10 even if a smaller value is passed."""
        result = progress_bar(1, 10, width=2)
        # The bar portion should have 10 chars (min width), not 2
        bar_part = result[result.index("[") + 1: result.index("]")]
        assert len(bar_part) == 10

    def test_default_width_when_none(self) -> None:
        """width=None computes from terminal_width (default 80 → 40 clamped)."""
        result = progress_bar(1, 2)
        assert "1/2" in result
        assert "50.0%" in result

    def test_percentage_one_decimal(self) -> None:
        """Percentage is formatted to 1 decimal place."""
        result = progress_bar(1, 3, width=30)
        assert "33.3%" in result


# ─── get_mtime() ────────────────────────────────────────────────────────────

class TestGetMtime:
    """get_mtime() returns the max mtime among a list of files."""

    def test_empty_list(self) -> None:
        """Empty file list → 0.0."""
        assert get_mtime([]) == 0.0

    def test_single_file(self, tmp_path: Path) -> None:
        """Single file → its mtime."""
        f = tmp_path / "a.c"
        f.write_text("x", encoding="utf-8")
        assert get_mtime([f]) == pytest.approx(f.stat().st_mtime)

    def test_multiple_returns_max(self, tmp_path: Path) -> None:
        """Multiple files → the maximum mtime."""
        old = tmp_path / "old.c"
        old.write_text("old", encoding="utf-8")
        time.sleep(0.05)
        new = tmp_path / "new.c"
        new.write_text("new", encoding="utf-8")
        assert get_mtime([old, new]) == pytest.approx(new.stat().st_mtime)
        assert get_mtime([new, old]) == pytest.approx(new.stat().st_mtime)

    def test_nonexistent_file_skipped(self, tmp_path: Path) -> None:
        """Non-existent files are skipped (OSError caught), not crashed."""
        real = tmp_path / "real.c"
        real.write_text("x", encoding="utf-8")
        fake = tmp_path / "nonexistent.c"
        # Should return real's mtime, ignoring the missing file
        assert get_mtime([real, fake]) == pytest.approx(real.stat().st_mtime)

    def test_all_nonexistent(self) -> None:
        """All files missing → 0.0."""
        assert get_mtime([Path("/nonexistent/a"), Path("/nonexistent/b")]) == 0.0


# ─── terminal_hyperlink() ───────────────────────────────────────────────────

class TestTerminalHyperlink:
    """terminal_hyperlink() creates OSC 8 clickable terminal links."""

    def test_default_display(self, tmp_path: Path) -> None:
        """When display=None, the path string itself is the display text."""
        f = tmp_path / "main.c"
        f.write_text("x", encoding="utf-8")
        link = terminal_hyperlink(f)
        # OSC 8 format: \x1b]8;;<uri>\x1b\\<display>\x1b]8;;\x1b\\
        assert link.startswith("\x1b]8;;")
        assert "\x1b\\" in link
        # Display text defaults to str(path)
        assert str(f) in link
        # URI is the file:// absolute path
        assert f.resolve().as_uri() in link

    def test_custom_display(self, tmp_path: Path) -> None:
        """Custom display text is used as the visible link text."""
        f = tmp_path / "main.c"
        f.write_text("x", encoding="utf-8")
        link = terminal_hyperlink(f, display="click here")
        assert "click here" in link
        # The custom display appears between the OSC 8 open and close sequences
        # Format: \x1b]8;;<uri>\x1b\\<display>\x1b]8;;\x1b\\
        # Verify the display text is the visible part (not the default str(path))
        open_seq = "\x1b]8;;"
        close_seq = "\x1b\\"
        # Find the display segment between first close and second open
        first_close = link.index(close_seq) + len(close_seq)
        second_open = link.rindex(open_seq)
        display_segment = link[first_close:second_open]
        assert display_segment == "click here"

    def test_osc8_terminator(self, tmp_path: Path) -> None:
        """Link ends with the OSC 8 closing sequence."""
        f = tmp_path / "x.c"
        f.write_text("x", encoding="utf-8")
        link = terminal_hyperlink(f, display="x")
        assert link.endswith("\x1b]8;;\x1b\\")


# ─── source_files_for() / watch_files_for() ────────────────────────────────

class TestSourceFilesFor:
    """source_files_for() locates .c source files for an exercise."""

    def test_single_source_field(self, isolated_workspace: Path) -> None:
        """'source' field → returns that single .c file path."""
        ex_dir = isolated_workspace / "exercises" / "01_simplest_c_program"
        (ex_dir / "simplest_c_program.c").write_text("x", encoding="utf-8")
        ex = {"name": "01_simplest_c_program", "path": "01_simplest_c_program",
              "source": "simplest_c_program.c"}
        files = source_files_for(ex, use_solutions=False)
        assert len(files) == 1
        assert files[0].name == "simplest_c_program.c"

    def test_sources_field_multiple(self, isolated_workspace: Path) -> None:
        """'sources' field (list) → returns multiple .c files."""
        ex_dir = isolated_workspace / "exercises" / "multi"
        ex_dir.mkdir(parents=True)
        (ex_dir / "a.c").write_text("x", encoding="utf-8")
        (ex_dir / "b.c").write_text("y", encoding="utf-8")
        ex = {"name": "multi", "path": "multi", "sources": ["a.c", "b.c"]}
        files = source_files_for(ex, use_solutions=False)
        assert len(files) == 2
        assert {f.name for f in files} == {"a.c", "b.c"}

    def test_glob_fallback(self, isolated_workspace: Path) -> None:
        """No 'source'/'sources' → glob all *.c sorted."""
        ex_dir = isolated_workspace / "exercises" / "globby"
        ex_dir.mkdir(parents=True)
        (ex_dir / "b.c").write_text("y", encoding="utf-8")
        (ex_dir / "a.c").write_text("x", encoding="utf-8")
        ex = {"name": "globby", "path": "globby"}
        files = source_files_for(ex, use_solutions=False)
        assert len(files) == 2
        assert files[0].name == "a.c"  # sorted
        assert files[1].name == "b.c"

    def test_nonexistent_dir(self) -> None:
        """Non-existent source dir → empty list (not crash)."""
        ex = {"name": "ghost", "path": "ghost", "source": "ghost.c"}
        assert source_files_for(ex, use_solutions=False) == []


class TestWatchFilesFor:
    """watch_files_for() returns build-relevant files for make modes."""

    def test_make_mode_includes_headers_and_makefile(
        self, isolated_workspace: Path
    ) -> None:
        """make/make+stdout mode → *.c + *.h + Makefile + *.mk."""
        ex_dir = isolated_workspace / "exercises" / "mkex"
        ex_dir.mkdir(parents=True)
        (ex_dir / "main.c").write_text("x", encoding="utf-8")
        (ex_dir / "header.h").write_text("y", encoding="utf-8")
        (ex_dir / "Makefile").write_text("all:\n", encoding="utf-8")
        (ex_dir / "common.mk").write_text("z", encoding="utf-8")
        (ex_dir / "notes.txt").write_text("ignore", encoding="utf-8")
        ex = {"name": "mkex", "path": "mkex", "source": "main.c",
              "mode": "make+stdout"}
        files = watch_files_for(ex, use_solutions=False)
        names = {f.name for f in files}
        assert "main.c" in names
        assert "header.h" in names
        assert "Makefile" in names
        assert "common.mk" in names
        assert "notes.txt" not in names  # non-build file excluded

    def test_non_make_mode_falls_back(self, isolated_workspace: Path) -> None:
        """stdout mode → falls back to source_files_for (just .c files)."""
        ex_dir = isolated_workspace / "exercises" / "stdoutex"
        ex_dir.mkdir(parents=True)
        (ex_dir / "main.c").write_text("x", encoding="utf-8")
        (ex_dir / "header.h").write_text("y", encoding="utf-8")
        ex = {"name": "stdoutex", "path": "stdoutex",
              "source": "main.c", "mode": "stdout"}
        files = watch_files_for(ex, use_solutions=False)
        names = {f.name for f in files}
        assert "main.c" in names
        assert "header.h" not in names  # not make mode, headers excluded
