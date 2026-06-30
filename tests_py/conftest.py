"""Shared pytest fixtures for clings self-tests.

Key challenge: clings.config computes module-level constants (ROOT,
CONFIG_PATH, STATE_FILE, PUBLIC_TEST_DIR, EXERCISES_DIR, PKG_CONFIG) from
Path.cwd() at import time. These drive exercise discovery and state
persistence. Tests MUST redirect them to a temp directory to avoid
polluting the real workspace and to ensure isolation.

The ``isolated_workspace`` fixture handles this by monkeypatching all
relevant constants to point under a per-test tmp_path, and chdir-ing
into it so any code that re-reads cwd also sees the temp root.
"""

from __future__ import annotations

import os
import textwrap
from pathlib import Path

import pytest


# ─── Sample Data ────────────────────────────────────────────────────────────

SAMPLE_CLINGS_TOML = """\
# Exercise metadata in exercises/*/exercises.toml (auto-discovered)

[[units]]
id = "unit0"
title = "C Primer"
lessons = "01-05"

[[units]]
id = "unit3"
title = "C Classicals"
lessons = "49-72"
"""

SAMPLE_EXERCISES_TOML = """\
[[exercises]]
name = "01_simplest_c_program"
title = "最简单的 C 程序 — Simplest C Program"
unit = "unit0"
lesson = 1
order = 1
mode = "stdout"
source = "simplest_c_program.c"

[[exercises.cases]]
stdin = ""
stdout = "Hello, World!\\n"

[[exercises]]
name = "49_dining-philosophers-sync"
title = "哲学家就餐问题 — Dining Philosophers"
unit = "unit3"
lesson = 49
order = 97
mode = "make+stdout"
source = "dining_philosophers.c"
binary = "dining_philosophers"
make_targets = ["all"]

[[exercises.cases]]
args = ["naive"]
exit_code = 2
stdout_contains = ["DEADLOCK DETECTED"]
"""

SAMPLE_STATE_FILE = """\
# Clings progress file. Remove this file to reset all progress.

current_exercise = 01_simplest_c_program

[done]
01_simplest_c_program
"""


# ─── Workspace Isolation Fixture ────────────────────────────────────────────

@pytest.fixture
def isolated_workspace(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> Path:
    """Create an isolated clings workspace under tmp_path.

    - Creates a minimal clings.toml + exercises/ tree.
    - Redirects clings.config module constants (ROOT, CONFIG_PATH,
      STATE_FILE, PUBLIC_TEST_DIR, EXERCISES_DIR, PKG_CONFIG, BUILD_DIR)
      to point under tmp_path.
    - chdir into tmp_path so cwd-relative code works.

    Returns the tmp_path (the new ROOT).
    """
    # Build a minimal exercises tree.
    ex_dir = tmp_path / "exercises" / "01_simplest_c_program"
    ex_dir.mkdir(parents=True)
    (ex_dir / "exercises.toml").write_text(SAMPLE_EXERCISES_TOML, encoding="utf-8")

    # clings.toml at the workspace root.
    (tmp_path / "clings.toml").write_text(SAMPLE_CLINGS_TOML, encoding="utf-8")

    # tests/ directory (empty by default; individual tests may populate).
    (tmp_path / "tests").mkdir()

    # Redirect module-level constants BEFORE importing/using config.
    # We patch on the clings.config module object so all consumers see the
    # redirected values.
    from clings import config as cfg

    state_file_path = tmp_path / ".clings-state.txt"
    monkeypatch.setattr(cfg, "ROOT", tmp_path)
    monkeypatch.setattr(cfg, "CONFIG_PATH", tmp_path / "clings.toml")
    monkeypatch.setattr(cfg, "STATE_FILE", state_file_path)
    monkeypatch.setattr(cfg, "BUILD_DIR", tmp_path / ".clings" / "build")
    monkeypatch.setattr(cfg, "PUBLIC_TEST_DIR", tmp_path / "tests")
    monkeypatch.setattr(cfg, "EXERCISES_DIR", tmp_path / "exercises")
    monkeypatch.setattr(cfg, "PKG_CONFIG", tmp_path / "clings.toml")

    # CRITICAL: state.py and utils.py do `from .config import STATE_FILE`
    # which binds the Path object AT IMPORT TIME (value copy, not reference).
    # monkeypatching cfg.STATE_FILE does NOT affect their already-bound copy.
    # We must patch each consumer module's STATE_FILE attribute directly.
    from clings import state as st
    monkeypatch.setattr(st, "STATE_FILE", state_file_path, raising=False)

    from clings import utils
    monkeypatch.setattr(utils, "STATE_FILE", state_file_path, raising=False)

    # compiler.py also value-copies ROOT and BUILD_DIR at import time.
    from clings import compiler as comp
    monkeypatch.setattr(comp, "ROOT", tmp_path, raising=False)
    monkeypatch.setattr(comp, "BUILD_DIR", tmp_path / ".clings" / "build", raising=False)

    # reset.py and list.py also import STATE_FILE directly.
    from clings.commands import reset as reset_mod
    monkeypatch.setattr(reset_mod, "STATE_FILE", state_file_path, raising=False)
    # list.py imports STATE_FILE too, but state tests don't exercise it.

    # chdir so any code reading Path.cwd() directly sees tmp_path.
    monkeypatch.chdir(tmp_path)

    # Clear the make cache so tests don't interfere with each other.
    # MAKE_CACHE is a module-level set that persists across tests; without
    # clearing, a cached build from one test would cause another test's
    # make step to be skipped, hiding failures.
    from clings import compiler as comp_mod
    comp_mod.MAKE_CACHE.clear()

    return tmp_path


@pytest.fixture
def workspace_with_state(isolated_workspace: Path) -> Path:
    """Isolated workspace pre-seeded with a progress state file."""
    (isolated_workspace / ".clings-state.txt").write_text(
        SAMPLE_STATE_FILE, encoding="utf-8"
    )
    return isolated_workspace


@pytest.fixture
def sample_config() -> dict:
    """An in-memory config dict matching SAMPLE_EXERCISES_TOML, no IO needed."""
    return {
        "units": [
            {"id": "unit0", "title": "C Primer", "lessons": "01-05"},
            {"id": "unit3", "title": "C Classicals", "lessons": "49-72"},
        ],
        "exercises": [
            {
                "name": "01_simplest_c_program",
                "title": "最简单的 C 程序 — Simplest C Program",
                "unit": "unit0",
                "lesson": 1,
                "order": 1,
                "mode": "stdout",
                "source": "simplest_c_program.c",
                "path": "01_simplest_c_program",
                "cases": [{"stdin": "", "stdout": "Hello, World!\n"}],
            },
            {
                "name": "49_dining-philosophers-sync",
                "title": "哲学家就餐问题 — Dining Philosophers",
                "unit": "unit3",
                "lesson": 49,
                "order": 97,
                "mode": "make+stdout",
                "source": "dining_philosophers.c",
                "binary": "dining_philosophers",
                "make_targets": ["all"],
                "path": "49_dining-philosophers-sync",
                "cases": [{"args": ["naive"], "exit_code": 2,
                           "stdout_contains": ["DEADLOCK DETECTED"]}],
            },
        ],
    }
