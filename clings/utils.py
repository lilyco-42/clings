"""Shared utilities — terminal helpers, file operations, and exercise reset."""

import os
import subprocess
import sys
from pathlib import Path

from .config import ROOT, source_dir_for


# ─── ANSI Constants ──────────────────────────────────────────────────────────

ANSI_RESET = "\x1b[0m"
ANSI_BOLD = "\x1b[1m"
ANSI_DIM = "\x1b[2m"
ANSI_RED = "\x1b[31m"
ANSI_GREEN = "\x1b[32m"
ANSI_YELLOW = "\x1b[33m"
ANSI_BLUE = "\x1b[34m"
ANSI_CYAN = "\x1b[36m"
ANSI_BOLD_RED = "\x1b[31;1m"
ANSI_BOLD_GREEN = "\x1b[32;1m"
ANSI_BOLD_CYAN = "\x1b[36;1m"


# ─── Terminal Helpers ─────────────────────────────────────────────────────────

def clear_screen() -> None:
    """Clear terminal screen and move cursor to top-left."""
    sys.stdout.write("\x1b[2J\x1b[H")
    sys.stdout.flush()


def terminal_width() -> int:
    """Get terminal width, defaulting to 80 if unavailable."""
    try:
        return os.get_terminal_size().columns
    except OSError:
        return 80


def progress_bar(n_done: int, total: int, width: int | None = None) -> str:
    """Render a text-based progress bar string."""
    if width is None:
        width = min(terminal_width() - 30, 40)
    width = max(width, 10)
    if total == 0:
        return ""
    filled = int(width * n_done / total)
    bar = "\u2588" * filled + "\u2591" * (width - filled)
    pct = 100.0 * n_done / total
    return f"Progress: [{bar}] {n_done}/{total} ({pct:.1f}%)"


def terminal_hyperlink(path: Path, display: str | None = None) -> str:
    """Create an OSC 8 terminal hyperlink for clickable file paths."""
    abs_path = path.resolve()
    if display is None:
        display = str(path)
    uri = abs_path.as_uri()
    return f"\x1b]8;;{uri}\x1b\\{display}\x1b]8;;\x1b\\"


# ─── File Helpers ────────────────────────────────────────────────────────────

def source_files_for(ex: dict, use_solutions: bool) -> list[Path]:
    """Get the list of source files for an exercise."""
    src_dir = source_dir_for(ex, use_solutions)
    if not src_dir.exists():
        return []
    if "sources" in ex:
        return [src_dir / s for s in ex["sources"]]
    elif "source" in ex:
        return [src_dir / ex["source"]]
    else:
        return sorted(src_dir.glob("*.c"))


def watch_files_for(ex: dict, use_solutions: bool) -> list[Path]:
    """Get files to watch for changes in watch mode.

    For make/make+stdout mode, this includes ALL build-relevant files in the
    source directory: *.c, *.h, Makefile, *.mk. This ensures that editing the
    Makefile or a header file triggers a rerun (a common blind spot — the
    previous implementation only watched the single ``source`` .c file).

    For other modes, falls back to ``source_files_for`` (just the .c files).
    """
    mode = ex.get("mode", "stdout")
    if mode not in ("make", "make+stdout"):
        return source_files_for(ex, use_solutions)
    src_dir = source_dir_for(ex, use_solutions)
    if not src_dir.exists():
        return []
    files: list[Path] = []
    for pattern in ("*.c", "*.h", "Makefile", "makefile", "GNUmakefile", "*.mk"):
        files.extend(sorted(src_dir.glob(pattern)))
    return files


def get_mtime(files: list[Path]) -> float:
    """Get the most recent modification time among a list of files."""
    mtime = 0.0
    for f in files:
        try:
            mtime = max(mtime, f.stat().st_mtime)
        except OSError:
            pass
    return mtime


def read_key_nonblocking() -> str | None:
    """Read a single keypress without blocking. Returns None if no key pressed."""
    if os.name == "nt":
        import msvcrt
        if msvcrt.kbhit():
            return msvcrt.getwch()
        return None
    else:
        import select as _select
        rlist, _, _ = _select.select([sys.stdin], [], [], 0)
        if rlist:
            return sys.stdin.read(1)
        return None


# ─── Exercise Reset ──────────────────────────────────────────────────────────

def reset_exercise(ex: dict) -> bool:
    """Reset an exercise file to its original state via git checkout."""
    src_dir = source_dir_for(ex, use_solutions=False)
    if not src_dir.exists():
        return False
    try:
        result = subprocess.run(
            ["git", "checkout", "--", str(src_dir)],
            cwd=ROOT, text=True, capture_output=True,
        )
        return result.returncode == 0
    except FileNotFoundError:
        return False
