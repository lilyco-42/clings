#!/usr/bin/env python3
"""Replace ALL remaining Chinese #error TODO lines with generic English."""
import re
from pathlib import Path

EXERCISES_DIR = Path(__file__).resolve().parent.parent / "exercises"
REPLACEMENT = '#error TODO: Fix this exercise. Run "clings hint" for help.'

count = 0
files_modified = 0
for c_file in sorted(EXERCISES_DIR.rglob("*.c")):
    text = c_file.read_text(encoding="utf-8")
    lines = text.splitlines(keepends=True)
    modified = False
    for i, line in enumerate(lines):
        stripped = line.strip()
        if stripped.startswith("#error TODO:") and re.search(r"[\u4e00-\u9fff]", stripped):
            indent = line[: len(line) - len(line.lstrip())]
            lines[i] = indent + REPLACEMENT + "\n"
            modified = True
            count += 1
    if modified:
        c_file.write_text("".join(lines), encoding="utf-8")
        rel = c_file.relative_to(EXERCISES_DIR.parent)
        print(f"  [OK] {rel}")
        files_modified += 1

print(f"\nFixed {count} Chinese #error lines in {files_modified} files")
