"""clings init — initialize exercises for a specific unit."""

import argparse
import shutil
import sys
import tomllib

from ..config import (
    ClingsError,
    EXERCISES_DIR,
    PKG_CONFIG,
    ROOT,
    UNIT_LESSON_RANGES,
    discover_exercises,
)


def cmd_init(args: argparse.Namespace) -> int:
    unit = args.unit or "unit1"
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
        print("error: exercises not found in clings package — reinstall clings",
              file=sys.stderr)
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

        for src_file in src_dir.iterdir():
            if src_file.is_dir():
                continue
            dst_file = dst_dir / src_file.name
            if dst_file.exists() and src_file.name.endswith(".c"):
                skipped_files += 1
                continue
            shutil.copy2(src_file, dst_file)
            copied_files += 1

        copied_dirs.add(ex_path)

    units_label = ", ".join(units_to_init)
    print(f"\n  \x1b[32;1m\u2705 Initialized {len(copied_dirs)} lesson directories "
          f"({len(selected)} exercises) for {units_label}\x1b[0m")
    print(f"  copied {copied_files} files, skipped {skipped_files} existing .c files")
    print(f"\n  Run \x1b[1mclings\x1b[0m to start!")
    if skipped_files:
        print(f"  (use \x1b[1mclings reset <exercise>\x1b[0m to restore individual files)")
    return 0
