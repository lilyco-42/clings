"""clings init — initialize exercises for a specific unit."""

import argparse
import os
import shutil
import sys
import tomllib
import tempfile
import zipfile
from pathlib import Path
from urllib.request import urlopen

from ..config import (
    EXERCISES_DIR,
    PKG_CONFIG,
    ROOT,
    UNIT_LESSON_RANGES,
    discover_exercises,
)

_IS_WINDOWS = os.name == "nt"
_OK = "ok" if _IS_WINDOWS else "\u2705"

GITEE_REPO = "https://gitee.com/lilyco42/clings/repository/archive/main.zip"


# Filenames a student edits to complete an exercise. `init` must never
# overwrite these once they exist (without --force): for make+stdout exercises
# the Makefile IS the assignment, so clobbering it silently would destroy work.
# Reference/config files (README.md, exercises.toml) are always refreshed.
_WORK_FILE_SUFFIXES = (".c", ".h", ".mk")
_WORK_FILE_NAMES = {"Makefile", "makefile", "GNUmakefile"}


def _download_exercises():
    """Download exercises from Gitee repo if not found locally."""
    print("  downloading exercises from gitee...")
    try:
        with urlopen(GITEE_REPO, timeout=30) as resp:
            data = resp.read()
        with tempfile.NamedTemporaryFile(suffix=".zip", delete=False) as f:
            f.write(data)
            tmp_path = f.name
        with zipfile.ZipFile(tmp_path) as zf:
            # archive contains a folder like "clings-xxx/"
            names = zf.namelist()
            prefix = names[0].split("/")[0] + "/" if names else ""
            # extract exercises/ and clings.toml
            for name in names:
                if "/exercises/" in name or name.endswith("/clings.toml"):
                    rel = name[len(prefix):] if name.startswith(prefix) else name
                    if rel:
                        target = ROOT / rel
                        target.parent.mkdir(parents=True, exist_ok=True)
                        if not name.endswith("/"):
                            with zf.open(name) as src, open(target, "wb") as dst:
                                dst.write(src.read())
        os.unlink(tmp_path)
        return True
    except Exception as e:
        print(f"  download failed: {e}", file=sys.stderr)
        return False


def _is_work_file(name: str) -> bool:
    """True if ``name`` is a student-editable work file protected from re-init."""
    return name.endswith(_WORK_FILE_SUFFIXES) or name in _WORK_FILE_NAMES


def cmd_init(args: argparse.Namespace) -> int:
    unit = args.unit or "unit1"
    force = getattr(args, "force", False)
    if unit == "all":
        units_to_init = list(UNIT_LESSON_RANGES.keys())
    elif unit in UNIT_LESSON_RANGES:
        units_to_init = [unit]
    else:
        available = ", ".join(list(UNIT_LESSON_RANGES.keys()) + ["all"])
        print(f"unknown unit: {unit!r} (available: {available})", file=sys.stderr)
        return 1

    pkg_exercises = EXERCISES_DIR
    if not pkg_exercises.exists():
        # try downloading from gitee
        if not _download_exercises():
            print("error: exercises not found — run `clings init` to download",
                  file=sys.stderr)
            return 1
        # re-check after download
        pkg_exercises = EXERCISES_DIR
        if not pkg_exercises.exists():
            print("error: exercises not found after download", file=sys.stderr)
            return 1

    target_exercises = ROOT / "exercises"
    target_config = ROOT / "clings.toml"

    # Generate clings.toml with only the requested units (merge if exists)
    if PKG_CONFIG.exists():
        with open(PKG_CONFIG, "rb") as cf:
            pkg_data = tomllib.load(cf)
        # Load existing target config or start fresh
        if target_config.exists():
            with open(target_config, "rb") as cf:
                existing = tomllib.load(cf)
            existing_ids = {u["id"] for u in existing.get("units", [])}
        else:
            existing = {"units": []}
            existing_ids = set()
        # Add new units from package config
        new_units = [u for u in pkg_data.get("units", [])
                     if u.get("id") in units_to_init and u.get("id") not in existing_ids]
        if new_units or not target_config.exists():
            all_units = existing.get("units", []) + new_units
            lines = ["# Exercise metadata in exercises/*/exercises.toml (auto-discovered)\n"]
            for u in all_units:
                lines.append(f'\n[[units]]\nid = "{u["id"]}"\ntitle = "{u["title"]}"\nlessons = "{u["lessons"]}"\n')
            target_config.write_text("".join(lines))
            if not existing_ids:
                print(f"  created clings.toml")
            elif new_units:
                print(f"  updated clings.toml (+{len(new_units)} unit)")

    target_exercises.mkdir(parents=True, exist_ok=True)

    # If source and target are the same dir (running from repo), skip copy
    same_dir = pkg_exercises.resolve() == target_exercises.resolve()

    # Discover exercises from the package's exercises directory
    # We need to find the root that contains the exercises/ directory
    pkg_root = pkg_exercises.parent
    all_pkg_exercises = discover_exercises(pkg_root)
    selected = [
        ex for ex in all_pkg_exercises
        if ex.get("unit") in units_to_init
    ]

    copied_dirs: set[str] = set()
    copied_files = 0
    skipped_files = 0

    for ex in selected:
        ex_path = ex.get("path", "")
        if not ex_path or ex_path in copied_dirs:
            continue

        src_dir = pkg_exercises / ex_path
        dst_dir = target_exercises / ex_path

        if not src_dir.exists():
            continue

        dst_dir.mkdir(parents=True, exist_ok=True)

        if not same_dir:
            for src_file in src_dir.iterdir():
                if src_file.is_dir():
                    continue
                dst_file = dst_dir / src_file.name
                # Preserve existing student work (source, headers, Makefile) so a
                # repeated `clings init` never destroys progress. `--force` opts in
                # to overwriting them.
                if dst_file.exists() and _is_work_file(src_file.name) and not force:
                    skipped_files += 1
                    continue
                shutil.copy2(src_file, dst_file)
                copied_files += 1

        copied_dirs.add(ex_path)

    units_label = ", ".join(units_to_init)
    print(f"\n  \x1b[32;1m{_OK} Initialized {len(copied_dirs)} lesson directories "
          f"({len(selected)} exercises) for {units_label}\x1b[0m")
    skipped_note = "existing work files (.c/.h/Makefile)" if not force else "files"
    print(f"  copied {copied_files} files, skipped {skipped_files} {skipped_note}")
    print(f"\n  Run \x1b[1mclings\x1b[0m to start!")
    if skipped_files:
        print(f"  (use \x1b[1mclings reset <exercise>\x1b[0m to restore a file, "
              f"or \x1b[1mclings init {unit} --force\x1b[0m to overwrite all)")
    return 0
