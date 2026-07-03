"""Configuration, paths, and exercise discovery."""

import os
import shutil
import tomllib
from pathlib import Path


# ─── Path Constants ──────────────────────────────────────────────────────────

PKG_DIR = Path(__file__).resolve().parent

# exercises & clings.toml may be in-package (pip install) or repo root (pip install -e .)
_exercises_in_pkg = PKG_DIR / "exercises"
_exercises_in_repo = PKG_DIR.parent / "exercises"
EXERCISES_DIR = _exercises_in_pkg if _exercises_in_pkg.exists() else _exercises_in_repo

_config_in_pkg = PKG_DIR / "clings.toml"
_config_in_repo = PKG_DIR.parent / "clings.toml"
PKG_CONFIG = _config_in_pkg if _config_in_pkg.exists() else _config_in_repo

ROOT = Path.cwd()
BUILD_DIR = ROOT / ".clings" / "build"
CONFIG_PATH = ROOT / "clings.toml"

# Test cases: prefer bundled tests/ inside the installed package (site-packages),
# fall back to the repo-root tests/ during development. Layout is flat:
#   tests/<exercise-name>.toml
# e.g. tests/49_dining-philosophers-sync.toml
#
# The fallback is anchored at PKG_DIR.parent (the repo root in an editable
# `pip install -e .` layout), NOT Path.cwd(). Anchoring at cwd was a latent
# bug: an editable install invoked from a scratch dir (as CI does, running
# from /tmp/clings-ci-unitN) would find neither PKG_DIR/tests nor cwd/tests
# and silently fall back to the inline `exercises.toml` cases — diverging from
# the authoritative, tamper-resistant tests/ source. Anchoring at the repo
# root makes editable/dev/CI use the same tests/ as a real pip install.
_pkg_tests = PKG_DIR / "tests"
_repo_tests = PKG_DIR.parent / "tests"
PUBLIC_TEST_DIR = _pkg_tests if _pkg_tests.exists() else _repo_tests

STATE_FILE = ROOT / ".clings-state.txt"
HIDDEN_TEST_ENV = "CLINGS_HIDDEN_TEST_DIR"
SOLUTIONS_ENV = "CLINGS_SOLUTIONS_DIR"


# ─── Exceptions ──────────────────────────────────────────────────────────────

class ClingsError(Exception):
    pass


# ─── TOML Loading ────────────────────────────────────────────────────────────

def load_toml(path: Path) -> dict:
    with path.open("rb") as f:
        return tomllib.load(f)


# ─── Exercise Discovery ──────────────────────────────────────────────────────

def discover_exercises(root: Path) -> list[dict]:
    """Scan exercises/**/exercises.toml for per-directory configs, return sorted list."""
    exercises_dir = root / "exercises"
    if not exercises_dir.exists():
        return []
    result = []
    for toml_path in sorted(exercises_dir.rglob("exercises.toml")):
        try:
            data = load_toml(toml_path)
        except Exception:
            continue
        ex_dir = toml_path.parent
        rel_path = str(ex_dir.relative_to(exercises_dir))
        for ex_data in data.get("exercises", []):
            ex = dict(ex_data)
            if "path" not in ex:
                ex["path"] = rel_path
            ex["cases"] = ex_data.get("cases", [])
            result.append(ex)
    result.sort(key=lambda e: (e.get("unit", ""), e.get("lesson", 0), e.get("order", 0)))
    return result


def load_config() -> dict:
    if not CONFIG_PATH.exists():
        raise ClingsError("missing clings.toml")
    config = load_toml(CONFIG_PATH)
    if "exercises" not in config or not config["exercises"]:
        config["exercises"] = discover_exercises(ROOT)
    return config


# ─── Compiler Detection ──────────────────────────────────────────────────────

def find_compiler() -> str | None:
    env_cc = os.environ.get("CC")
    if env_cc:
        return env_cc
    for candidate in ["gcc", "cc", "clang"]:
        found = shutil.which(candidate)
        if found:
            return found
    if os.name == "nt":
        mingw = Path("D:/env/tools/MinGW/bin/gcc.exe")
        if mingw.exists():
            return str(mingw)
    return None


# ─── Exercise Query Helpers ──────────────────────────────────────────────────

def exercises(config: dict) -> list[dict]:
    return config.get("exercises", [])


def find_exercise(config: dict, selector: str) -> dict:
    matches = [
        ex for ex in exercises(config)
        if selector in {ex["name"], ex["unit"], str(ex["lesson"])}
        or ex["name"].startswith(selector)
    ]
    if not matches:
        raise ClingsError(f"no exercise matches {selector!r}")
    if len(matches) > 1 and all(ex["name"] != selector for ex in matches):
        names = ", ".join(ex["name"] for ex in matches[:8])
        raise ClingsError(f"ambiguous selector {selector!r}: {names}")
    return next((ex for ex in matches if ex["name"] == selector), matches[0])


def select_exercises(config: dict, selector: str | None) -> list[dict]:
    all_ex = exercises(config)
    if selector is None:
        return all_ex
    if selector.startswith("unit"):
        selected = [ex for ex in all_ex if ex["unit"] == selector]
        # A unit selector that matches nothing (e.g. the unit isn't initialized
        # in this workspace, or a typo) must be an error — NOT a silent empty
        # set. Returning [] here made `clings check unit3` print
        # "all 0 exercise(s) passed" and exit 0 (a dangerous false green).
        if not selected:
            raise ClingsError(f"no exercises match {selector!r}")
        return selected
    if selector.isdigit():
        selected = [ex for ex in all_ex if ex["lesson"] == int(selector)]
        if selected:
            return selected
    needle = selector.lower()
    selected = [
        ex for ex in all_ex
        if ex["name"].lower().startswith(needle)
        or needle in ex["title"].lower()
    ]
    if selected:
        return selected
    return [find_exercise(config, selector)]


# ─── Source/Test File Helpers ────────────────────────────────────────────────

def test_files_for(ex: dict, include_hidden: bool) -> list[Path]:
    """Locate test case files for an exercise.

    Layout is flat: tests/<exercise-name>.toml
    Public tests come from PUBLIC_TEST_DIR (bundled in package or repo-root).
    Hidden tests (if any) come from CLINGS_HIDDEN_TEST_DIR env var.
    """
    rel = Path(f"{ex['name']}.toml")
    files = []
    public = PUBLIC_TEST_DIR / rel
    if public.exists():
        files.append(public)
    hidden_root = os.environ.get(HIDDEN_TEST_ENV)
    if include_hidden and hidden_root:
        hidden = Path(hidden_root) / rel
        if hidden.exists():
            files.append(hidden)
    return files


def source_dir_for(ex: dict, use_solutions: bool) -> Path:
    if use_solutions:
        base = Path(os.environ.get(SOLUTIONS_ENV, ROOT / "solutions"))
    else:
        base = ROOT / "exercises"
    return base / ex["path"]


# ─── Unit Lesson Ranges ─────────────────────────────────────────────────────

def _load_unit_lesson_ranges() -> dict[str, tuple[int, int]]:
    """Parse [[units]] from clings.toml to get unit -> (start, end) lesson ranges."""
    if not PKG_CONFIG.exists():
        return {}
    with open(PKG_CONFIG, "rb") as f:
        data = tomllib.load(f)
    ranges = {}
    for unit in data.get("units", []):
        uid = unit.get("id", "")
        lessons = unit.get("lessons", "")
        if "-" in lessons:
            start, end = lessons.split("-", 1)
            ranges[uid] = (int(start), int(end))
    return ranges


UNIT_LESSON_RANGES = _load_unit_lesson_ranges()


# ─── Version ────────────────────────────────────────────────────────────────

def get_version() -> str:
    """Read version from pyproject.toml bundled with the package."""
    pyproject = PKG_DIR / "pyproject.toml"
    if not pyproject.exists():
        pyproject = PKG_DIR.parent / "pyproject.toml"
    try:
        with open(pyproject, "rb") as f:
            return tomllib.load(f).get("project", {}).get("version", "unknown")
    except FileNotFoundError:
        return "unknown"
