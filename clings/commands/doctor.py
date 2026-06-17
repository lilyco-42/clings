"""clings doctor — check development environment."""

import argparse
import os
import shutil
import subprocess
import sys

from ..config import BUILD_DIR, CONFIG_PATH, ROOT, find_compiler


def cmd_doctor(args: argparse.Namespace) -> int:
    print(f"python: {sys.version.split()[0]}")
    gcc = find_compiler()
    make = shutil.which("make")
    print(f"gcc: {gcc or 'missing'}")
    print(f"make: {make or 'missing'}")
    print(f"config: {CONFIG_PATH.relative_to(ROOT)}")
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    print(f"build dir: {BUILD_DIR.relative_to(ROOT)}")
    if not gcc:
        return 1
    smoke = BUILD_DIR / "doctor_smoke.c"
    binary = BUILD_DIR / ("doctor_smoke.exe" if os.name == "nt" else "doctor_smoke")
    smoke.write_text("int main(void){return 0;}\n", encoding="ascii")
    proc = subprocess.run([gcc, str(smoke), "-o", str(binary)], text=True, capture_output=True)
    if proc.returncode != 0:
        print("compiler smoke test: failed")
        if proc.stderr.strip():
            print(proc.stderr.strip())
        return 1
    print("compiler smoke test: ok")
    return 0
