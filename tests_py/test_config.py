"""Tests for clings.config — exercise discovery, selection, and path helpers.

Two layers:
  - Pure-function tests (select_exercises, find_exercise): use the in-memory
    sample_config fixture, no filesystem needed.
  - Filesystem tests (discover_exercises, load_toml, test_files_for,
    _load_unit_lesson_ranges): use isolated_workspace to redirect module
    constants to a temp directory.
"""

from __future__ import annotations

import os
from pathlib import Path

import pytest

from clings.config import (
    ClingsError,
    discover_exercises,
    exercises,
    find_exercise,
    load_toml,
    select_exercises,
    _load_unit_lesson_ranges,
)
# Alias to avoid pytest collecting the real function as a test (it matches test_*)
from clings.config import test_files_for as _test_files_for
from clings import config as cfg


# ═══════════════════════════════════════════════════════════════════════════
# Layer 1: Pure-function tests (no filesystem)
# ═══════════════════════════════════════════════════════════════════════════

class TestSelectExercises:
    """select_exercises() is the selector parser — core CLI routing logic."""

    def test_none_returns_all(self, sample_config: dict) -> None:
        """selector=None returns the full exercise list."""
        result = select_exercises(sample_config, None)
        assert len(result) == 2
        assert result[0]["name"] == "01_simplest_c_program"
        assert result[1]["name"] == "49_dining-philosophers-sync"

    def test_unit_prefix(self, sample_config: dict) -> None:
        """'unit0' matches all exercises in unit0."""
        result = select_exercises(sample_config, "unit0")
        assert len(result) == 1
        assert result[0]["name"] == "01_simplest_c_program"

    def test_unit3_prefix(self, sample_config: dict) -> None:
        """'unit3' matches all exercises in unit3."""
        result = select_exercises(sample_config, "unit3")
        assert len(result) == 1
        assert result[0]["name"] == "49_dining-philosophers-sync"

    def test_lesson_digit_hit(self, sample_config: dict) -> None:
        """'49' (digit) matches lesson 49."""
        result = select_exercises(sample_config, "49")
        assert len(result) == 1
        assert result[0]["name"] == "49_dining-philosophers-sync"

    def test_lesson_digit_no_hit_falls_through(self, sample_config: dict) -> None:
        """Digit with no lesson match falls through to name/title search,
        then to find_exercise (which raises ClingsError if nothing matches)."""
        with pytest.raises(ClingsError, match="no exercise matches"):
            select_exercises(sample_config, "999")

    def test_name_prefix(self, sample_config: dict) -> None:
        """'49_dining' matches by name prefix."""
        result = select_exercises(sample_config, "49_dining")
        assert len(result) == 1
        assert result[0]["name"] == "49_dining-philosophers-sync"

    def test_name_prefix_case_insensitive(self, sample_config: dict) -> None:
        """Name prefix matching is case-insensitive."""
        result = select_exercises(sample_config, "49_DINING")
        assert len(result) == 1

    def test_title_substring(self, sample_config: dict) -> None:
        """Selector matches as a substring of the title (case-insensitive)."""
        result = select_exercises(sample_config, "philosopher")
        assert len(result) == 1
        assert result[0]["name"] == "49_dining-philosophers-sync"

    def test_title_substring_chinese(self, sample_config: dict) -> None:
        """Chinese title substrings also match."""
        result = select_exercises(sample_config, "哲学家")
        assert len(result) == 1
        assert result[0]["name"] == "49_dining-philosophers-sync"

    def test_fallback_find_exercise_raises(self, sample_config: dict) -> None:
        """Non-matching selector falls through to find_exercise, which raises."""
        with pytest.raises(ClingsError, match="no exercise matches"):
            select_exercises(sample_config, "totally-nonexistent")


class TestFindExercise:
    """find_exercise() does exact-or-prefix matching with ambiguity detection."""

    def test_exact_name_hit(self, sample_config: dict) -> None:
        """Exact name match returns that exercise."""
        ex = find_exercise(sample_config, "01_simplest_c_program")
        assert ex["name"] == "01_simplest_c_program"

    def test_unit_match(self, sample_config: dict) -> None:
        """Matching by unit string returns first match."""
        ex = find_exercise(sample_config, "unit0")
        assert ex["unit"] == "unit0"

    def test_lesson_string_match(self, sample_config: dict) -> None:
        """Matching by lesson number (as string) returns that exercise."""
        ex = find_exercise(sample_config, "49")
        assert ex["lesson"] == 49

    def test_name_prefix_match(self, sample_config: dict) -> None:
        """Prefix match returns the exercise."""
        ex = find_exercise(sample_config, "49_dining")
        assert ex["name"] == "49_dining-philosophers-sync"

    def test_miss_raises(self, sample_config: dict) -> None:
        """No match raises ClingsError."""
        with pytest.raises(ClingsError, match="no exercise matches"):
            find_exercise(sample_config, "does-not-exist")


# ═══════════════════════════════════════════════════════════════════════════
# Layer 2: Filesystem tests (isolated_workspace)
# ═══════════════════════════════════════════════════════════════════════════

class TestDiscoverExercises:
    """discover_exercises() scans exercises/**/exercises.toml."""

    def test_empty_dir(self, tmp_path: Path) -> None:
        """No exercises/ directory → empty list."""
        assert discover_exercises(tmp_path) == []

    def test_finds_and_sorts(self, isolated_workspace: Path) -> None:
        """Discovers exercises and sorts by (unit, lesson, order)."""
        result = discover_exercises(isolated_workspace)
        assert len(result) == 2
        # Sorted: unit0 lesson 1 first, then unit3 lesson 49
        assert result[0]["lesson"] == 1
        assert result[1]["lesson"] == 49

    def test_path_backfill(self, isolated_workspace: Path) -> None:
        """Exercises without explicit 'path' get it backfilled from dir name."""
        result = discover_exercises(isolated_workspace)
        for ex in result:
            assert "path" in ex
            assert ex["path"] == "01_simplest_c_program" or "49_" in ex["path"]

    def test_cases_inlined(self, isolated_workspace: Path) -> None:
        """Inline [[exercises.cases]] are attached to the exercise dict."""
        result = discover_exercises(isolated_workspace)
        ex = next(e for e in result if e["name"] == "01_simplest_c_program")
        assert len(ex["cases"]) == 1
        assert ex["cases"][0]["stdout"] == "Hello, World!\n"

    def test_malformed_toml_skipped(self, isolated_workspace: Path) -> None:
        """A malformed exercises.toml is silently skipped (not crash)."""
        bad_dir = isolated_workspace / "exercises" / "99_broken"
        bad_dir.mkdir(parents=True)
        (bad_dir / "exercises.toml").write_text("this is not valid toml {{{{", encoding="utf-8")
        result = discover_exercises(isolated_workspace)
        # Still finds the 2 valid ones, skips the broken one
        assert len(result) == 2


class TestLoadToml:
    """load_toml() reads and parses a TOML file."""

    def test_valid_file(self, tmp_path: Path) -> None:
        """A valid TOML file loads into a dict."""
        f = tmp_path / "test.toml"
        f.write_text('key = "value"\n[num]\nx = 1\n', encoding="utf-8")
        data = load_toml(f)
        assert data["key"] == "value"
        assert data["num"]["x"] == 1

    def test_empty_file(self, tmp_path: Path) -> None:
        """An empty TOML file loads to an empty dict."""
        f = tmp_path / "empty.toml"
        f.write_text("", encoding="utf-8")
        assert load_toml(f) == {}

    def test_invalid_file_raises(self, tmp_path: Path) -> None:
        """A malformed TOML file raises an exception (tomllib.TOMLDecodeError)."""
        f = tmp_path / "bad.toml"
        f.write_text("not = valid = toml\n", encoding="utf-8")
        with pytest.raises(Exception):
            load_toml(f)


class TestTestFilesFor:
    """test_files_for() locates public and hidden test case files."""

    def test_public_file_exists(self, isolated_workspace: Path) -> None:
        """Public test file in tests/ is found."""
        ex = {"name": "01_simplest_c_program"}
        test_file = isolated_workspace / "tests" / "01_simplest_c_program.toml"
        test_file.write_text('[[cases]]\nstdout = ""\n', encoding="utf-8")
        files = _test_files_for(ex, include_hidden=False)
        assert len(files) == 1
        assert files[0].name == "01_simplest_c_program.toml"

    def test_no_public_file(self, isolated_workspace: Path) -> None:
        """No test file → empty list."""
        ex = {"name": "nonexistent"}
        assert _test_files_for(ex, include_hidden=False) == []

    def test_hidden_file_included(self, isolated_workspace: Path,
                                  monkeypatch: pytest.MonkeyPatch) -> None:
        """When include_hidden=True and CLINGS_HIDDEN_TEST_DIR is set,
        hidden test files are appended after public ones."""
        hidden_dir = isolated_workspace / "hidden_tests"
        hidden_dir.mkdir()
        (hidden_dir / "01_simplest_c_program.toml").write_text(
            '[[cases]]\nstdout = "hidden"\n', encoding="utf-8"
        )
        monkeypatch.setenv("CLINGS_HIDDEN_TEST_DIR", str(hidden_dir))

        ex = {"name": "01_simplest_c_program"}
        # Also create a public file so both are returned
        (isolated_workspace / "tests" / "01_simplest_c_program.toml").write_text(
            '[[cases]]\nstdout = "public"\n', encoding="utf-8"
        )
        files = _test_files_for(ex, include_hidden=True)
        assert len(files) == 2
        assert files[0].parent.name == "tests"       # public first
        assert files[1].parent.name == "hidden_tests"  # hidden second

    def test_hidden_not_included_when_flag_false(
        self, isolated_workspace: Path, monkeypatch: pytest.MonkeyPatch
    ) -> None:
        """include_hidden=False ignores hidden dir even if env var is set."""
        hidden_dir = isolated_workspace / "hidden_tests"
        hidden_dir.mkdir()
        (hidden_dir / "01_simplest_c_program.toml").write_text(
            '[[cases]]\nstdout = "hidden"\n', encoding="utf-8"
        )
        monkeypatch.setenv("CLINGS_HIDDEN_TEST_DIR", str(hidden_dir))

        ex = {"name": "01_simplest_c_program"}
        files = _test_files_for(ex, include_hidden=False)
        assert len(files) == 0


class TestLoadUnitLessonRanges:
    """_load_unit_lesson_ranges() parses clings.toml [[units]] lessons field."""

    def test_parses_ranges(self, monkeypatch: pytest.MonkeyPatch,
                           tmp_path: Path) -> None:
        """Valid clings.toml yields unit→(start,end) mapping."""
        clings_toml = tmp_path / "clings.toml"
        clings_toml.write_text(
            '[[units]]\nid = "unit0"\nlessons = "01-05"\n\n'
            '[[units]]\nid = "unit3"\nlessons = "49-72"\n',
            encoding="utf-8",
        )
        monkeypatch.setattr(cfg, "PKG_CONFIG", clings_toml)
        ranges = _load_unit_lesson_ranges()
        assert ranges["unit0"] == (1, 5)
        assert ranges["unit3"] == (49, 72)

    def test_no_config_returns_empty(self, monkeypatch: pytest.MonkeyPatch,
                                      tmp_path: Path) -> None:
        """Missing PKG_CONFIG → empty dict."""
        monkeypatch.setattr(cfg, "PKG_CONFIG", tmp_path / "nonexistent.toml")
        assert _load_unit_lesson_ranges() == {}

    def test_malformed_lessons_skipped(self, monkeypatch: pytest.MonkeyPatch,
                                        tmp_path: Path) -> None:
        """A unit with no '-' in lessons is skipped (not crashed)."""
        clings_toml = tmp_path / "clings.toml"
        clings_toml.write_text(
            '[[units]]\nid = "unitX"\nlessons = "single"\n', encoding="utf-8"
        )
        monkeypatch.setattr(cfg, "PKG_CONFIG", clings_toml)
        ranges = _load_unit_lesson_ranges()
        assert "unitX" not in ranges
