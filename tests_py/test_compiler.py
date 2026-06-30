"""Unit tests for clings.compiler — pure-function layer (no subprocess).

Covers normalize() (the recently-added trim_trailing_ws feature) and
_make_source_mtime_signature() (build-cache mtime computation).

These are the highest-ROI tests: they guard the output-comparison logic
that every exercise's pass/fail depends on, and they run in milliseconds
with no compiler or filesystem dependencies beyond tmp_path.
"""

from __future__ import annotations

import os
import time
from pathlib import Path

import pytest

from clings.compiler import normalize, _make_source_mtime_signature, _collect_cases


# ─── normalize() ────────────────────────────────────────────────────────────

class TestNormalize:
    """normalize() is the output-comparison normalization core.

    Invariants under test:
      1. CRLF → LF always (regardless of trim).
      2. Default (trim=False): trailing whitespace preserved (backward compat).
      3. trim=True: trailing whitespace stripped from each line.
      4. trim=True: leading whitespace PRESERVED (only trailing touched).
      5. Trailing newline NEVER stripped (POSIX text-file convention).
      6. CRLF + trim compose correctly.
    """

    def test_normalize_crlf_to_lf(self) -> None:
        """CRLF line endings are normalized to LF, always."""
        assert normalize("hello\r\nworld\r\n") == "hello\nworld\n"

    def test_normalize_crlf_single_line(self) -> None:
        """Single CRLF line also normalized."""
        assert normalize("hello\r\n") == "hello\n"

    def test_normalize_no_crlf_unchanged(self) -> None:
        """Pure LF input passes through unchanged."""
        assert normalize("hello\nworld\n") == "hello\nworld\n"

    def test_normalize_empty_string(self) -> None:
        """Empty string is a no-op."""
        assert normalize("") == ""

    def test_normalize_default_keeps_trailing_ws(self) -> None:
        """Default (trim=False): trailing spaces preserved — backward compat.

        This is the critical regression guard: changing the default to trim
        would silently relax grading for all 97 existing exercises.
        """
        assert normalize("hello   \nworld \n") == "hello   \nworld \n"

    def test_normalize_default_keeps_trailing_tabs(self) -> None:
        """Trailing tabs also preserved by default."""
        assert normalize("hello\t\nworld\t\t\n") == "hello\t\nworld\t\t\n"

    def test_normalize_trim_strips_trailing_spaces(self) -> None:
        """trim=True: trailing spaces removed from each line."""
        assert normalize("hello   \nworld \n", trim_trailing_ws=True) == "hello\nworld\n"

    def test_normalize_trim_strips_trailing_tabs(self) -> None:
        """trim=True: trailing tabs also removed (rstrip covers all ws)."""
        assert normalize("hello\t\nworld\t\t\n", trim_trailing_ws=True) == "hello\nworld\n"

    def test_normalize_trim_strips_mixed_ws(self) -> None:
        """trim=True: mixed trailing spaces+tabs removed."""
        assert normalize("a \t \nb\t \n", trim_trailing_ws=True) == "a\nb\n"

    def test_normalize_trim_keeps_leading_spaces(self) -> None:
        """trim=True: leading whitespace is PRESERVED.

        Only trailing whitespace is stripped — leading indentation is
        semantically meaningful in many outputs (e.g. tree diagrams).
        """
        assert normalize("  hello \n    world \n", trim_trailing_ws=True) == "  hello\n    world\n"

    def test_normalize_trim_keeps_internal_spaces(self) -> None:
        """trim=True: internal spaces preserved, only trailing stripped."""
        assert normalize("hello   world \n", trim_trailing_ws=True) == "hello   world\n"

    def test_normalize_trim_keeps_trailing_newline(self) -> None:
        """trim=True: the trailing newline is NOT stripped.

        POSIX defines a text file as ending with a newline. Stripping it
        would relax a legitimate C-engineering teaching requirement.
        """
        assert normalize("hello\n", trim_trailing_ws=True) == "hello\n"
        assert normalize("hello \n", trim_trailing_ws=True) == "hello\n"

    def test_normalize_trim_keeps_multiple_trailing_newlines(self) -> None:
        """Multiple trailing newlines preserved (not collapsed)."""
        assert normalize("hello\n\n\n", trim_trailing_ws=True) == "hello\n\n\n"

    def test_normalize_trim_empty_lines_become_empty(self) -> None:
        """trim=True: lines with only whitespace become empty (rstrip)."""
        assert normalize("hello\n   \nworld\n", trim_trailing_ws=True) == "hello\n\nworld\n"

    def test_normalize_trim_empty_string(self) -> None:
        """trim=True on empty string is a no-op."""
        assert normalize("", trim_trailing_ws=True) == ""

    def test_normalize_trim_crlf_combined(self) -> None:
        """CRLF normalization + trim compose correctly.

        Order: CRLF→LF first, then per-line rstrip. This matches the
        implementation (replace then split/join).
        """
        raw = "hello \r\nworld \r\n"
        assert normalize(raw, trim_trailing_ws=True) == "hello\nworld\n"

    def test_normalize_trim_crlf_with_trailing_ws_before_newline(self) -> None:
        """'hello  \\r\\n' → trim → 'hello\\n' (space before CRLF stripped)."""
        assert normalize("hello  \r\n", trim_trailing_ws=True) == "hello\n"

    @pytest.mark.parametrize("raw,expected", [
        ("hello\n", "hello\n"),
        ("hello\r\n", "hello\n"),
        ("hello \n", "hello\n"),
        ("hello \r\n", "hello\n"),
        ("  hello \t\n", "  hello\n"),
        ("", ""),
        ("\n", "\n"),
        (" \n", "\n"),
    ])
    def test_normalize_trim_parametrized(self, raw: str, expected: str) -> None:
        """Parametrized smoke test covering common input patterns."""
        assert normalize(raw, trim_trailing_ws=True) == expected

    @pytest.mark.parametrize("raw,expected", [
        ("hello\n", "hello\n"),
        ("hello\r\n", "hello\n"),
        ("hello \n", "hello \n"),  # default keeps trailing
        ("", ""),
    ])
    def test_normalize_default_parametrized(self, raw: str, expected: str) -> None:
        """Parametrized smoke test for default (non-trim) behavior."""
        assert normalize(raw) == expected


# ─── _make_source_mtime_signature() ─────────────────────────────────────────

class TestMakeSourceMtimeSignature:
    """_make_source_mtime_signature() computes a build-input mtime fingerprint.

    It scans a source directory for build-relevant files (*.c, *.h, Makefile,
    *.mk) and returns the max mtime. This drives the make-cache invalidation
    in check_make / check_make_stdout: if no build input changed, make is
    skipped. Correctness here prevents both stale-build bugs (signature too
    coarse) and needless rebuilds (signature too fine).
    """

    def test_empty_dir_returns_zero(self, tmp_path: Path) -> None:
        """An empty directory yields signature 0.0 (no files to stat)."""
        assert _make_source_mtime_signature(tmp_path) == 0.0

    def test_nonexistent_dir_returns_zero(self, tmp_path: Path) -> None:
        """A non-existent directory yields 0.0 (glob finds nothing)."""
        assert _make_source_mtime_signature(tmp_path / "nonexistent") == 0.0

    def test_single_c_file(self, tmp_path: Path) -> None:
        """A single .c file: signature equals its mtime."""
        c_file = tmp_path / "main.c"
        c_file.write_text("int main(void){return 0;}", encoding="utf-8")
        expected = c_file.stat().st_mtime
        assert _make_source_mtime_signature(tmp_path) == pytest.approx(expected)

    def test_multiple_files_returns_max(self, tmp_path: Path) -> None:
        """Multiple files: signature is the MAX mtime (most recent change)."""
        old_file = tmp_path / "old.c"
        new_file = tmp_path / "new.c"
        old_file.write_text("old", encoding="utf-8")
        # Force new_file to be strictly newer by touching it after a delay.
        time.sleep(0.05)
        new_file.write_text("new", encoding="utf-8")

        sig = _make_source_mtime_signature(tmp_path)
        assert sig == pytest.approx(new_file.stat().st_mtime)
        assert sig > old_file.stat().st_mtime

    def test_includes_makefile(self, tmp_path: Path) -> None:
        """Makefile changes are tracked (case-insensitive variants)."""
        c_file = tmp_path / "main.c"
        c_file.write_text("x", encoding="utf-8")
        makefile = tmp_path / "Makefile"
        time.sleep(0.05)
        makefile.write_text("all:\n", encoding="utf-8")

        sig = _make_source_mtime_signature(tmp_path)
        assert sig == pytest.approx(makefile.stat().st_mtime)

    def test_includes_header_files(self, tmp_path: Path) -> None:
        """Header (.h) files are tracked — they affect compilation."""
        c_file = tmp_path / "main.c"
        c_file.write_text("x", encoding="utf-8")
        h_file = tmp_path / "header.h"
        time.sleep(0.05)
        h_file.write_text("#pragma once", encoding="utf-8")

        sig = _make_source_mtime_signature(tmp_path)
        assert sig == pytest.approx(h_file.stat().st_mtime)

    def test_includes_mk_fragments(self, tmp_path: Path) -> None:
        """.mk include fragments are tracked."""
        c_file = tmp_path / "main.c"
        c_file.write_text("x", encoding="utf-8")
        mk_file = tmp_path / "common.mk"
        time.sleep(0.05)
        mk_file.write_text("CFLAGS += -O2\n", encoding="utf-8")

        sig = _make_source_mtime_signature(tmp_path)
        assert sig == pytest.approx(mk_file.stat().st_mtime)

    def test_ignores_non_build_files(self, tmp_path: Path) -> None:
        """Non-build files (*.txt, *.md, README) do NOT affect signature."""
        c_file = tmp_path / "main.c"
        c_file.write_text("x", encoding="utf-8")
        c_mtime = c_file.stat().st_mtime

        # Add a .txt file LATER than the .c file.
        time.sleep(0.05)
        (tmp_path / "notes.txt").write_text("irrelevant", encoding="utf-8")
        (tmp_path / "README.md").write_text("# readme", encoding="utf-8")

        sig = _make_source_mtime_signature(tmp_path)
        # Signature should equal the .c mtime, NOT the later .txt/.md.
        assert sig == pytest.approx(c_mtime)

    def test_gnu_makefile_variant(self, tmp_path: Path) -> None:
        """GNUmakefile (GNU make's preferred name) is also tracked."""
        c_file = tmp_path / "main.c"
        c_file.write_text("x", encoding="utf-8")
        gnu_mk = tmp_path / "GNUmakefile"
        time.sleep(0.05)
        gnu_mk.write_text("all:\n", encoding="utf-8")

        sig = _make_source_mtime_signature(tmp_path)
        assert sig == pytest.approx(gnu_mk.stat().st_mtime)

    def test_lowercase_makefile_variant(self, tmp_path: Path) -> None:
        """Lowercase 'makefile' is also tracked."""
        c_file = tmp_path / "main.c"
        c_file.write_text("x", encoding="utf-8")
        lower_mk = tmp_path / "makefile"
        time.sleep(0.05)
        lower_mk.write_text("all:\n", encoding="utf-8")

        sig = _make_source_mtime_signature(tmp_path)
        assert sig == pytest.approx(lower_mk.stat().st_mtime)


# ─── _collect_cases() ───────────────────────────────────────────────────────

class TestCollectCases:
    """_collect_cases() determines which test cases apply to an exercise.

    Priority: external tests/ files > hidden test files > inline cases
    from exercises.toml. This drives grading authority — the bundled tests/
    are authoritative, inline cases are for student reference only.
    """

    def test_external_priority_over_inline(self, isolated_workspace: Path) -> None:
        """External tests/ file takes priority over inline [[exercises.cases]].

        This is the critical grading-authority invariant: students cannot
        tamper with the tests/ file (it lives outside their exercise dir),
        so it must override the inline cases they can see.
        """
        # Create an external test file with different content than inline
        test_file = isolated_workspace / "tests" / "01_simplest_c_program.toml"
        test_file.write_text(
            '[[cases]]\nstdout = "from external tests"\n',
            encoding="utf-8",
        )
        # Inline cases (from exercises.toml) say "Hello, World!"
        ex = {
            "name": "01_simplest_c_program",
            "cases": [{"stdout": "Hello, World!\\n"}],
        }
        cases = _collect_cases(ex, include_hidden=False)
        assert len(cases) == 1
        assert cases[0]["stdout"] == "from external tests"

    def test_inline_fallback_when_no_external(self, isolated_workspace: Path) -> None:
        """No external test file → falls back to inline cases."""
        ex = {
            "name": "no_external_file",
            "cases": [{"stdout": "inline case\\n"}],
        }
        cases = _collect_cases(ex, include_hidden=False)
        assert len(cases) == 1
        assert cases[0]["stdout"] == "inline case\\n"

    def test_no_cases_returns_empty(self, isolated_workspace: Path) -> None:
        """No external file and no inline cases → empty list."""
        ex = {"name": "empty_ex"}
        assert _collect_cases(ex, include_hidden=False) == []

    def test_hidden_merged_after_public(
        self, isolated_workspace: Path, monkeypatch: pytest.MonkeyPatch
    ) -> None:
        """Hidden test files are appended after public ones."""
        # Public test
        (isolated_workspace / "tests" / "01_simplest_c_program.toml").write_text(
            '[[cases]]\nstdout = "public"\n', encoding="utf-8"
        )
        # Hidden test
        hidden_dir = isolated_workspace / "hidden"
        hidden_dir.mkdir()
        (hidden_dir / "01_simplest_c_program.toml").write_text(
            '[[cases]]\nstdout = "hidden"\n', encoding="utf-8"
        )
        monkeypatch.setenv("CLINGS_HIDDEN_TEST_DIR", str(hidden_dir))

        ex = {"name": "01_simplest_c_program", "cases": []}
        cases = _collect_cases(ex, include_hidden=True)
        assert len(cases) == 2
        assert cases[0]["stdout"] == "public"
        assert cases[1]["stdout"] == "hidden"

    def test_hidden_not_included_when_flag_false(
        self, isolated_workspace: Path, monkeypatch: pytest.MonkeyPatch
    ) -> None:
        """include_hidden=False → only public tests, hidden ignored."""
        (isolated_workspace / "tests" / "01_simplest_c_program.toml").write_text(
            '[[cases]]\nstdout = "public"\n', encoding="utf-8"
        )
        hidden_dir = isolated_workspace / "hidden"
        hidden_dir.mkdir()
        (hidden_dir / "01_simplest_c_program.toml").write_text(
            '[[cases]]\nstdout = "hidden"\n', encoding="utf-8"
        )
        monkeypatch.setenv("CLINGS_HIDDEN_TEST_DIR", str(hidden_dir))

        ex = {"name": "01_simplest_c_program", "cases": []}
        cases = _collect_cases(ex, include_hidden=False)
        assert len(cases) == 1
        assert cases[0]["stdout"] == "public"
