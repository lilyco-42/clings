"""Guard: exercises.toml inline cases must mirror tests/<name>.toml exactly.

clings keeps test cases in two places on purpose:

  - ``tests/<name>.toml`` — the *authoritative* grading source, bundled into
    the wheel (``force-include``) so it lives in site-packages, out of a
    student's reach (they can't edit it to cheat).
  - the inline ``[[exercises.cases]]`` block inside each
    ``exercises/<path>/exercises.toml`` — a *copy* shipped into the student
    workspace so ``clings tests`` can show what's expected.

If those two ever drift, ``clings tests`` would show students something
different from what actually grades them — a silent, confusing trap. This test
compares the two for every exercise that has an external tests file and fails
the build on any mismatch, so drift is caught in CI rather than by a student.

It reads the real repository layout (not the isolated_workspace fixture), so it
also implicitly checks that the on-disk data is well-formed TOML.
"""

from __future__ import annotations

import tomllib
from pathlib import Path

import pytest

# tests_py/ lives directly under the repo root.
REPO_ROOT = Path(__file__).resolve().parents[1]
EXERCISES_DIR = REPO_ROOT / "exercises"
TESTS_DIR = REPO_ROOT / "tests"


def _load(path: Path) -> dict:
    with path.open("rb") as f:
        return tomllib.load(f)


def _collect() -> list[tuple[str, list, list]]:
    """(name, inline_cases, external_cases) for every exercise with a tests file."""
    if not EXERCISES_DIR.exists() or not TESTS_DIR.exists():
        return []
    rows: list[tuple[str, list, list]] = []
    for toml_path in sorted(EXERCISES_DIR.rglob("exercises.toml")):
        data = _load(toml_path)
        for ex in data.get("exercises", []):
            name = ex.get("name")
            if not name:
                continue
            external_file = TESTS_DIR / f"{name}.toml"
            if not external_file.exists():
                # unit0-2 exercises grade from inline cases only (no external
                # tests file) — nothing to cross-check for those.
                continue
            inline = ex.get("cases", [])
            external = _load(external_file).get("cases", [])
            rows.append((name, inline, external))
    return rows


_ROWS = _collect()


@pytest.mark.skipif(not _ROWS, reason="no exercises with external tests/ files found")
@pytest.mark.parametrize(
    "name,inline,external",
    _ROWS,
    ids=[r[0] for r in _ROWS],
)
def test_inline_cases_match_external(name: str, inline: list, external: list) -> None:
    """exercises.toml inline cases must be byte-for-byte identical to tests/."""
    assert inline, f"{name}: exercises.toml has no inline [[exercises.cases]] copy"
    assert external, f"{name}: tests/{name}.toml has no [[cases]]"
    assert inline == external, (
        f"{name}: inline exercises.toml cases differ from tests/{name}.toml.\n"
        f"tests/ is authoritative; the inline block is the student-facing copy "
        f"shown by `clings tests` and MUST stay identical.\n"
        f"  inline  : {inline}\n"
        f"  external: {external}"
    )


def test_every_unit3_exercise_has_external_tests() -> None:
    """Every unit3 (49-72) exercise must ship an authoritative tests/ file.

    unit3 is graded via make+stdout against tests/<name>.toml; a missing file
    would silently fall back to inline cases (weaker guarantee), so we require
    the external file to exist.
    """
    if not EXERCISES_DIR.exists() or not TESTS_DIR.exists():
        pytest.skip("repo layout not present")
    missing: list[str] = []
    for toml_path in sorted(EXERCISES_DIR.rglob("exercises.toml")):
        for ex in _load(toml_path).get("exercises", []):
            if ex.get("unit") == "unit3":
                name = ex.get("name", "")
                if not (TESTS_DIR / f"{name}.toml").exists():
                    missing.append(name)
    assert not missing, f"unit3 exercises missing tests/<name>.toml: {missing}"
