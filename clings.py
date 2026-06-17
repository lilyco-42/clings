#!/usr/bin/env python3
import argparse
import os
import shutil
import subprocess
import sys
import time
import tomllib
from pathlib import Path


PKG_DIR = Path(__file__).resolve().parent
ROOT = Path.cwd()
BUILD_DIR = ROOT / ".clings" / "build"
CONFIG_PATH = ROOT / "clings.toml"
PUBLIC_TEST_DIR = ROOT / "tests" / "public"
STATE_FILE = ROOT / ".clings-state.txt"
HIDDEN_TEST_ENV = "CLINGS_HIDDEN_TEST_DIR"
SOLUTIONS_ENV = "CLINGS_SOLUTIONS_DIR"
MAKE_CACHE = set()


class ClingsError(Exception):
    pass


def load_toml(path: Path) -> dict:
    with path.open("rb") as f:
        return tomllib.load(f)


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
        return [ex for ex in all_ex if ex["unit"] == selector]
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


def test_files_for(ex: dict, include_hidden: bool) -> list[Path]:
    rel = Path(ex["unit"]) / f"{ex['name']}.toml"
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


def compile_exercise(ex: dict, use_solutions: bool) -> Path:
    src_dir = source_dir_for(ex, use_solutions)
    if not src_dir.exists():
        raise ClingsError(f"missing source directory: {src_dir.relative_to(ROOT)}")
    if "sources" in ex:
        c_files = [src_dir / source for source in ex["sources"]]
    elif "source" in ex:
        c_files = [src_dir / ex["source"]]
    else:
        c_files = sorted(src_dir.glob("*.c"))
    missing = [path for path in c_files if not path.exists()]
    if missing:
        names = ", ".join(str(path.relative_to(ROOT)) for path in missing)
        raise ClingsError(f"missing source file(s): {names}")
    if not c_files:
        raise ClingsError(f"no .c files in {src_dir.relative_to(ROOT)}")
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    suffix = ".exe" if os.name == "nt" else ""
    binary = BUILD_DIR / f"{ex['name']}{suffix}"
    compiler = ex.get("compiler") or find_compiler()
    if not compiler:
        raise ClingsError("missing C compiler: install gcc/clang or set CC=/path/to/compiler")
    cflags = ex.get("cflags", ["-std=c11", "-Wall", "-Wextra", "-pedantic", "-O2"])
    cmd = [
        compiler, *[str(flag) for flag in cflags],
        *[str(path) for path in c_files],
        *[str(flag) for flag in ex.get("ldflags", [])],
        "-lm", "-o", str(binary),
    ]
    proc = subprocess.run(cmd, cwd=ROOT, text=True, capture_output=True)
    if proc.returncode != 0:
        raise ClingsError(
            f"compile failed for {ex['name']}\n"
            f"$ {' '.join(cmd)}\n{proc.stderr.strip()}"
        )
    return binary


def normalize(text: str) -> str:
    return text.replace("\r\n", "\n")


def _collect_cases(ex: dict, include_hidden: bool) -> list[dict]:
    """Collect test cases from inline exercise data and/or external test files."""
    cases = list(ex.get("cases", []))
    for test_file in test_files_for(ex, include_hidden):
        data = load_toml(test_file)
        cases.extend(data.get("cases", []))
    return cases


def run_cases(ex: dict, binary: Path, include_hidden: bool) -> None:
    cases = _collect_cases(ex, include_hidden)
    if not cases:
        raise ClingsError(f"no test cases found for {ex['name']}")
    case_no = 0
    for case in cases:
        case_no += 1
        if case.get("compile_only", False):
            continue
        stdin = case.get("stdin", "")
        expected = case.get("stdout", "")
        args = [str(arg) for arg in case.get("args", [])]
        timeout = float(case.get("timeout", 2.0))
        proc = subprocess.run(
            [str(binary), *args],
            input=stdin,
            text=True,
            capture_output=True,
            timeout=timeout,
        )
        actual = normalize(proc.stdout)
        if proc.returncode != int(case.get("exit_code", 0)):
            raise ClingsError(
                f"{ex['name']} case {case_no} exited {proc.returncode}\n"
                f"stderr:\n{proc.stderr.strip()}"
            )
        if actual != normalize(expected):
            raise ClingsError(
                f"{ex['name']} case {case_no} output mismatch\n"
                f"stdin:\n{stdin}"
                f"expected:\n{expected}"
                f"actual:\n{actual}"
            )


def check_return(ex: dict, binary: Path) -> None:
    expected = int(ex.get("expected_return", 0))
    stdin_text = ex.get("stdin", "")
    proc = subprocess.run(
        [str(binary)],
        input=stdin_text,
        text=True,
        capture_output=True,
        timeout=float(ex.get("timeout", 2.0)),
    )
    if proc.returncode != expected:
        raise ClingsError(
            f"{ex['name']}: expected return {expected}, got {proc.returncode}"
            + (f"\nstderr:\n{proc.stderr.strip()}" if proc.stderr.strip() else "")
        )


def check_one(ex: dict, use_solutions: bool, include_hidden: bool) -> None:
    mode = ex.get("mode", "stdout")
    if mode == "make":
        check_make(ex, use_solutions)
        return
    if mode == "compile":
        compile_exercise(ex, use_solutions)
        return
    if mode == "return":
        binary = compile_exercise(ex, use_solutions)
        check_return(ex, binary)
        return
    binary = compile_exercise(ex, use_solutions)
    run_cases(ex, binary, include_hidden)


def check_make(ex: dict, use_solutions: bool) -> None:
    src_dir = source_dir_for(ex, use_solutions)
    if not src_dir.exists():
        raise ClingsError(f"missing source directory: {src_dir.relative_to(ROOT)}")
    targets = ex.get("make_targets", ["test"])
    env = os.environ.copy()
    env.setdefault("CC", find_compiler() or "cc")
    for target in targets:
        cache_key = (str(src_dir.resolve()), target, use_solutions)
        if cache_key in MAKE_CACHE:
            continue
        cmd = ["make", target]
        proc = subprocess.run(
            cmd,
            cwd=src_dir,
            text=True,
            capture_output=True,
            timeout=float(ex.get("timeout", 120.0)),
            env=env,
        )
        if proc.returncode != 0:
            raise ClingsError(
                f"make target failed for {ex['name']}\n"
                f"$ {' '.join(cmd)} (cwd {src_dir})\n"
                f"{proc.stdout[-4000:]}\n{proc.stderr[-4000:]}"
            )
        MAKE_CACHE.add(cache_key)


def cmd_list(args: argparse.Namespace) -> int:
    config = load_config()
    selected = select_exercises(config, args.selector)
    all_ex = exercises(config)
    done_names: set[str] = set()
    if STATE_FILE.exists():
        state = WatchState(all_ex)
        done_names = state._done
    n_done = sum(1 for ex in selected if ex["name"] in done_names)
    for ex in selected:
        mode = ex.get("mode", "stdout")
        marker = "\x1b[32m\u2714\x1b[0m" if ex["name"] in done_names else "\x1b[31m\u2022\x1b[0m"
        print(f"  {marker} {ex['unit']:5} {ex['lesson']:02d} {mode:8} {ex['name']:32} {ex['title']}")
    total = len(selected)
    print(f"\n  {_progress_bar(n_done, total)}")
    return 0


def cmd_hint(args: argparse.Namespace) -> int:
    config = load_config()
    if args.exercise:
        ex = find_exercise(config, args.exercise)
    else:
        ex = _next_pending_exercise(config)
        if ex is None:
            print("all exercises completed!")
            return 0
    print(ex.get("hint", "No hint yet."))
    return 0


def cmd_run(args: argparse.Namespace) -> int:
    import random as _random
    config = load_config()
    if not args.exercise or args.exercise == "next":
        ex = _next_pending_exercise(config)
        if ex is None:
            print("all exercises completed!")
            return 0
    elif args.exercise == "random":
        all_ex = exercises(config)
        pending = [e for e in all_ex if not WatchState(all_ex).is_done(e)]
        if not pending:
            pending = all_ex
        ex = _random.choice(pending)
    else:
        ex = find_exercise(config, args.exercise)
    mode = ex.get("mode", "stdout")
    use_solutions = args.solutions

    if mode == "make":
        check_make(ex, use_solutions)
        print(f"\n\x1b[32;1m\u2705 ok {ex['name']}\x1b[0m")
        return 0

    binary = compile_exercise(ex, use_solutions)

    if mode == "compile":
        print(f"\x1b[32;1m\u2705 ok {ex['name']} (compiled successfully)\x1b[0m")
        return 0

    if mode == "return":
        expected = int(ex.get("expected_return", 0))
        stdin_text = ex.get("stdin", "")
        proc = subprocess.run(
            [str(binary)], input=stdin_text, text=True,
            capture_output=True, timeout=float(ex.get("timeout", 2.0)),
        )
        if proc.stdout:
            print(proc.stdout, end="")
        print(f"\n\x1b[90m(exit code: {proc.returncode})\x1b[0m")
        if proc.returncode != expected:
            print(f"\x1b[31;1m\u274c {ex['name']}: expected return {expected}, got {proc.returncode}\x1b[0m")
            return 1
        print(f"\x1b[32;1m\u2705 ok {ex['name']}\x1b[0m")
        return 0

    # mode == "stdout": run first case, show output, then verify all
    all_cases = _collect_cases(ex, args.hidden)
    if not all_cases:
        raise ClingsError(f"no test cases found for {ex['name']}")
    first_case = None
    for case in all_cases:
        if not case.get("compile_only", False):
            first_case = case
            break

    stdin_text = first_case.get("stdin", "") if first_case else ""
    proc = subprocess.run(
        [str(binary)], input=stdin_text, text=True,
        capture_output=True, timeout=float(first_case.get("timeout", 2.0)) if first_case else 2.0,
    )
    if proc.stdout:
        print(proc.stdout, end="")
    if proc.stderr.strip():
        print(f"\x1b[33m{proc.stderr.strip()}\x1b[0m", file=sys.stderr)

    # now verify all cases
    try:
        run_cases(ex, binary, args.hidden)
    except ClingsError as exc:
        print(f"\n\x1b[31;1m\u274c {ex['name']} FAILED\x1b[0m\n{exc}", file=sys.stderr)
        return 1
    print(f"\n\x1b[32;1m\u2705 ok {ex['name']}\x1b[0m")
    return 0


def cmd_check(args: argparse.Namespace) -> int:
    config = load_config()
    selected = select_exercises(config, args.selector)
    if args.solutions and not Path(os.environ.get(SOLUTIONS_ENV, ROOT / "solutions")).exists():
        print(
            f"solutions are hidden; set {SOLUTIONS_ENV} to the private solutions directory",
            file=sys.stderr,
        )
        return 1
    total = len(selected)
    for index, ex in enumerate(selected, 1):
        label = f"[{index}/{total}] {ex['name']}"
        try:
            check_one(ex, args.solutions, args.hidden)
        except Exception as exc:
            print(f"{label} FAILED\n{exc}", file=sys.stderr)
            return 1
        print(f"{label} ok")
    print(f"all {total} exercise(s) passed")
    return 0


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


# ─────────────────────────────────────────────────────────────────────────────
# Watch Mode Infrastructure
# ─────────────────────────────────────────────────────────────────────────────

_ANSI_RESET = "\x1b[0m"
_ANSI_BOLD = "\x1b[1m"
_ANSI_DIM = "\x1b[2m"
_ANSI_RED = "\x1b[31m"
_ANSI_GREEN = "\x1b[32m"
_ANSI_YELLOW = "\x1b[33m"
_ANSI_BLUE = "\x1b[34m"
_ANSI_CYAN = "\x1b[36m"
_ANSI_BOLD_RED = "\x1b[31;1m"
_ANSI_BOLD_GREEN = "\x1b[32;1m"
_ANSI_BOLD_CYAN = "\x1b[36;1m"


def _clear_screen() -> None:
    sys.stdout.write("\x1b[2J\x1b[H")
    sys.stdout.flush()


def _terminal_width() -> int:
    try:
        return os.get_terminal_size().columns
    except OSError:
        return 80


def _progress_bar(n_done: int, total: int, width: int | None = None) -> str:
    if width is None:
        width = min(_terminal_width() - 30, 40)
    width = max(width, 10)
    if total == 0:
        return ""
    filled = int(width * n_done / total)
    bar = "\u2588" * filled + "\u2591" * (width - filled)
    pct = 100.0 * n_done / total
    return f"Progress: [{bar}] {n_done}/{total} ({pct:.1f}%)"


def _terminal_hyperlink(path: Path, display: str | None = None) -> str:
    abs_path = path.resolve()
    if display is None:
        display = str(path)
    uri = abs_path.as_uri()
    return f"\x1b]8;;{uri}\x1b\\{display}\x1b]8;;\x1b\\"


def _source_files_for(ex: dict, use_solutions: bool) -> list[Path]:
    src_dir = source_dir_for(ex, use_solutions)
    if not src_dir.exists():
        return []
    if "sources" in ex:
        return [src_dir / s for s in ex["sources"]]
    elif "source" in ex:
        return [src_dir / ex["source"]]
    else:
        return sorted(src_dir.glob("*.c"))


def _get_mtime(files: list[Path]) -> float:
    mtime = 0.0
    for f in files:
        try:
            mtime = max(mtime, f.stat().st_mtime)
        except OSError:
            pass
    return mtime


def _read_key_nonblocking() -> str | None:
    if os.name == "nt":
        import msvcrt
        if msvcrt.kbhit():
            ch = msvcrt.getwch()
            return ch
        return None
    else:
        import select as _select
        rlist, _, _ = _select.select([sys.stdin], [], [], 0)
        if rlist:
            ch = sys.stdin.read(1)
            return ch
        return None


class WatchState:
    def __init__(self, exercises: list[dict]) -> None:
        self._exercises = exercises
        self._name_to_idx: dict[str, int] = {
            ex["name"]: i for i, ex in enumerate(exercises)
        }
        self._done: set[str] = set()
        self._current_idx: int = 0
        self._load()

    def _load(self) -> None:
        if not STATE_FILE.exists():
            return
        try:
            text = STATE_FILE.read_text(encoding="utf-8")
        except OSError:
            return
        current_name: str | None = None
        in_done = False
        for line in text.splitlines():
            stripped = line.strip()
            if stripped.startswith("#") or not stripped:
                continue
            if stripped == "[done]":
                in_done = True
                continue
            if not in_done:
                if stripped.startswith("current_exercise"):
                    _, _, val = stripped.partition("=")
                    current_name = val.strip()
            else:
                if stripped in self._name_to_idx:
                    self._done.add(stripped)
        if current_name and current_name in self._name_to_idx:
            self._current_idx = self._name_to_idx[current_name]
        else:
            self._find_first_pending()

    def _find_first_pending(self) -> None:
        for i, ex in enumerate(self._exercises):
            if ex["name"] not in self._done:
                self._current_idx = i
                return
        self._current_idx = len(self._exercises) - 1

    def save(self) -> None:
        lines = [
            "# Clings progress file. Remove this file to reset all progress.\n",
            "\n",
            f"current_exercise = {self.current_exercise()['name']}\n",
            "\n",
            "[done]\n",
        ]
        for ex in self._exercises:
            if ex["name"] in self._done:
                lines.append(f"{ex['name']}\n")
        try:
            STATE_FILE.write_text("".join(lines), encoding="utf-8")
        except OSError:
            pass

    def current_exercise(self) -> dict:
        return self._exercises[self._current_idx]

    @property
    def current_index(self) -> int:
        return self._current_idx

    @property
    def total(self) -> int:
        return len(self._exercises)

    @property
    def n_done(self) -> int:
        return len(self._done)

    def is_done(self, ex: dict) -> bool:
        return ex["name"] in self._done

    def mark_done(self) -> None:
        self._done.add(self.current_exercise()["name"])
        self.save()

    def mark_pending(self, name: str) -> None:
        self._done.discard(name)
        self.save()

    def advance_next(self) -> bool:
        start = self._current_idx
        for offset in range(1, self.total):
            idx = (start + offset) % self.total
            if self._exercises[idx]["name"] not in self._done:
                self._current_idx = idx
                self.save()
                return True
        return False

    def jump_to(self, name: str) -> bool:
        if name in self._name_to_idx:
            self._current_idx = self._name_to_idx[name]
            self.save()
            return True
        return False

    def all_done(self) -> bool:
        return len(self._done) >= self.total

    def exercises_with_status(self) -> list[tuple[dict, bool]]:
        return [(ex, ex["name"] in self._done) for ex in self._exercises]


class WatchRenderer:
    def __init__(self, state: WatchState) -> None:
        self._state = state

    def render_success(self, ex: dict) -> None:
        _clear_screen()
        s = self._state
        print(f"{_ANSI_BOLD_GREEN}{_progress_bar(s.n_done, s.total)}{_ANSI_RESET}")
        print()
        src_files = _source_files_for(ex, False)
        rel_path = src_files[0].relative_to(ROOT) if src_files else ex["name"]
        link = _terminal_hyperlink(src_files[0], str(rel_path)) if src_files else str(rel_path)
        print(f"  {_ANSI_BOLD_GREEN}\u2705 Exercise done!{_ANSI_RESET}  {ex['name']}")
        print(f"  {_ANSI_DIM}File: {link}{_ANSI_RESET}")
        if s.all_done():
            print(f"\n  {_ANSI_BOLD_GREEN}\U0001f389 Congratulations! All {s.total} exercises completed!{_ANSI_RESET}")
            print(f"\n  {_ANSI_DIM}Press q to quit.{_ANSI_RESET}")
        else:
            print(f"\n  {_ANSI_DIM}Commands: n:next  h:hint  l:list  x:reset  q:quit{_ANSI_RESET}")

    def render_failure(self, ex: dict, error: str) -> None:
        _clear_screen()
        s = self._state
        print(f"{_ANSI_YELLOW}{_progress_bar(s.n_done, s.total)}{_ANSI_RESET}")
        print()
        src_files = _source_files_for(ex, False)
        rel_path = src_files[0].relative_to(ROOT) if src_files else ex["name"]
        link = _terminal_hyperlink(src_files[0], str(rel_path)) if src_files else str(rel_path)
        print(f"  {_ANSI_BOLD_RED}\u274c Current: {ex['name']}{_ANSI_RESET}")
        print(f"  {_ANSI_DIM}File: {link}{_ANSI_RESET}")
        print(f"  {_ANSI_DIM}Title: {ex['title']}{_ANSI_RESET}")
        print()
        for line in error.splitlines()[:30]:
            print(f"  {line}")
        if len(error.splitlines()) > 30:
            print(f"  {_ANSI_DIM}... (output truncated){_ANSI_RESET}")
        print(f"\n  {_ANSI_DIM}Commands: h:hint  l:list  r:rerun  x:reset  q:quit{_ANSI_RESET}")

    def render_hint(self, ex: dict) -> None:
        hint = ex.get("hint", "No hint available for this exercise.")
        print(f"\n  {_ANSI_BOLD_CYAN}\U0001f4a1 Hint:{_ANSI_RESET}")
        for line in hint.splitlines():
            print(f"  {_ANSI_CYAN}{line}{_ANSI_RESET}")
        print()

    def render_list(self, state: WatchState, filter_mode: str = "all",
                    cursor: int = 0, search_query: str = "",
                    search_active: bool = False) -> None:
        _clear_screen()
        mode_label = f"search: {search_query}" if search_active else f"filter: {filter_mode}"
        print(f"{_ANSI_BOLD}  Exercise List{_ANSI_RESET}  "
              f"{_ANSI_DIM}({mode_label}){_ANSI_RESET}")
        print(f"  {_progress_bar(state.n_done, state.total)}")
        print(f"  {_ANSI_DIM}j/k:\u2191\u2193  d:done p:pending a:all  "
              f"s:search  r:reset  Enter:jump  q:back{_ANSI_RESET}")
        print()
        items = state.exercises_with_status()
        visible: list[tuple[int, dict, bool]] = []
        for i, (ex, done) in enumerate(items):
            if filter_mode == "done" and not done:
                continue
            if filter_mode == "pending" and done:
                continue
            if search_query and search_query.lower() not in ex["name"].lower():
                continue
            visible.append((i, ex, done))
        for vi, (i, ex, done) in enumerate(visible):
            marker = f"{_ANSI_GREEN}\u2714{_ANSI_RESET}" if done else f"{_ANSI_RED}\u2022{_ANSI_RESET}"
            is_cursor = vi == cursor
            prefix = f"{_ANSI_BOLD_CYAN}\u25b6{_ANSI_RESET}" if is_cursor else " "
            name = ex["name"]
            title = ex["title"]
            lesson = f"L{ex['lesson']:02d}"
            if is_cursor:
                print(f"  {prefix} {marker} {lesson} {_ANSI_BOLD}{name:32s}{_ANSI_RESET} {_ANSI_DIM}{title}{_ANSI_RESET}")
            else:
                print(f"  {prefix} {marker} {lesson} {name:32s} {_ANSI_DIM}{title}{_ANSI_RESET}")
        if not visible:
            print(f"  {_ANSI_DIM}(no exercises match){_ANSI_RESET}")
        if search_active:
            print(f"\n  {_ANSI_BOLD_CYAN}/{search_query}\u2588{_ANSI_RESET}", end="", flush=True)


def _reset_exercise(ex: dict) -> bool:
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


def cmd_watch(args: argparse.Namespace) -> int:
    config = load_config()
    selected = select_exercises(config, args.selector)
    if not selected:
        print("no exercises match the selector", file=sys.stderr)
        return 1

    state = WatchState(selected)
    renderer = WatchRenderer(state)
    use_solutions = args.solutions
    include_hidden = args.hidden
    manual_run = getattr(args, "manual_run", False)
    edit_cmd = getattr(args, "edit_cmd", None) or os.environ.get("CLINGS_EDITOR")

    def _open_in_editor(ex: dict) -> None:
        if not edit_cmd:
            return
        src_files = _source_files_for(ex, use_solutions)
        if not src_files:
            return
        file_path = str(src_files[0])
        try:
            parts = edit_cmd.split()
            subprocess.Popen([*parts, file_path], cwd=ROOT,
                             stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        except OSError:
            pass

    last_mtime: float = 0.0
    exercise_passed: bool = False
    last_error: str = ""
    show_hint: bool = False
    in_list_mode: bool = False
    list_filter: str = "all"
    needs_render: bool = True

    if os.name != "nt":
        import tty
        import termios
        old_settings = termios.tcgetattr(sys.stdin.fileno())
        tty.setcbreak(sys.stdin.fileno())

    def _restore_terminal() -> None:
        if os.name != "nt":
            import termios as _termios
            _termios.tcsetattr(sys.stdin.fileno(), _termios.TCSADRAIN, old_settings)
        sys.stdout.write("\x1b[?25h")
        sys.stdout.flush()

    def _run_current() -> tuple[bool, str]:
        ex = state.current_exercise()
        try:
            check_one(ex, use_solutions, include_hidden)
            return True, ""
        except Exception as exc:
            return False, str(exc)

    def _check_file_changed() -> bool:
        nonlocal last_mtime
        ex = state.current_exercise()
        src_files = _source_files_for(ex, use_solutions)
        current_mtime = _get_mtime(src_files)
        if current_mtime > last_mtime:
            last_mtime = current_mtime
            return True
        return False

    def _do_run() -> None:
        nonlocal exercise_passed, last_error, show_hint, needs_render
        passed, error = _run_current()
        exercise_passed = passed
        last_error = error
        show_hint = False
        needs_render = True
        if passed:
            state.mark_done()

    def _render() -> None:
        nonlocal needs_render
        if not needs_render:
            return
        needs_render = False
        ex = state.current_exercise()
        if exercise_passed:
            renderer.render_success(ex)
        else:
            renderer.render_failure(ex, last_error)
        if show_hint:
            renderer.render_hint(ex)

    list_cursor: int = 0
    list_search_query: str = ""
    list_search_active: bool = False

    def _visible_items() -> list[tuple[int, dict, bool]]:
        items = state.exercises_with_status()
        result = []
        for i, (ex, done) in enumerate(items):
            if list_filter == "done" and not done:
                continue
            if list_filter == "pending" and done:
                continue
            if list_search_query and list_search_query.lower() not in ex["name"].lower():
                continue
            result.append((i, ex, done))
        return result

    def _render_list() -> None:
        renderer.render_list(state, list_filter, list_cursor, list_search_query, list_search_active)

    try:
        _open_in_editor(state.current_exercise())
        _check_file_changed()
        _do_run()

        list_needs_render: bool = True

        while True:
            if in_list_mode:
                if list_needs_render:
                    _render_list()
                    list_needs_render = False
                time.sleep(0.1)
                key = _read_key_nonblocking()
                if key is None:
                    continue
                list_needs_render = True
                visible = _visible_items()
                n_visible = len(visible)

                if list_search_active:
                    if key == "\x1b" or key == "\r" or key == "\n":
                        list_search_active = False
                    elif key == "\x7f" or key == "\x08":
                        list_search_query = list_search_query[:-1]
                        list_cursor = 0
                    elif key.isprintable() and len(key) == 1:
                        list_search_query += key
                        list_cursor = 0
                    continue

                if key == "q":
                    in_list_mode = False
                    list_search_query = ""
                    list_search_active = False
                    needs_render = True
                elif key == "\x1b":
                    in_list_mode = False
                    list_search_query = ""
                    list_search_active = False
                    needs_render = True
                elif key == "j" or key == "\x1b[B":
                    if n_visible > 0:
                        list_cursor = min(list_cursor + 1, n_visible - 1)
                elif key == "k" or key == "\x1b[A":
                    list_cursor = max(list_cursor - 1, 0)
                elif key == "g":
                    list_cursor = 0
                elif key == "G":
                    list_cursor = max(n_visible - 1, 0)
                elif key == "\r" or key == "\n":
                    if visible:
                        idx, ex_item, _ = visible[list_cursor]
                        state.jump_to(ex_item["name"])
                        in_list_mode = False
                        list_search_query = ""
                        list_search_active = False
                        last_mtime = 0.0
                        _do_run()
                elif key == "d":
                    list_filter = "done"
                    list_cursor = 0
                elif key == "p":
                    list_filter = "pending"
                    list_cursor = 0
                elif key == "a":
                    list_filter = "all"
                    list_cursor = 0
                elif key == "s" or key == "/":
                    list_search_active = True
                    list_search_query = ""
                    list_cursor = 0
                elif key == "r":
                    if visible:
                        _, ex_item, _ = visible[list_cursor]
                        _reset_exercise(ex_item)
                        state.mark_pending(ex_item["name"])
                continue

            _render()

            time.sleep(0.1)
            key = _read_key_nonblocking()

            if key == "q":
                break
            elif key == "n":
                if exercise_passed:
                    if state.advance_next():
                        _open_in_editor(state.current_exercise())
                        last_mtime = 0.0
                        _do_run()
                        needs_render = True
            elif key == "h":
                show_hint = not show_hint
                needs_render = True
            elif key == "l":
                in_list_mode = True
                list_needs_render = True
                list_filter = "all"
                list_cursor = 0
                list_search_query = ""
                list_search_active = False
            elif key == "r":
                last_mtime = 0.0
                _do_run()
                needs_render = True
            elif key == "x":
                ex = state.current_exercise()
                if _reset_exercise(ex):
                    state.mark_pending(ex["name"])
                    last_mtime = 0.0
                    _do_run()
                    needs_render = True
            elif key == "c":
                _clear_screen()
                print(f"  {_ANSI_BOLD}Checking all exercises...{_ANSI_RESET}\n")
                total = state.total
                for i, (ex, _) in enumerate(state.exercises_with_status()):
                    label = f"[{i+1}/{total}]"
                    try:
                        check_one(ex, use_solutions, include_hidden)
                        state._done.add(ex["name"])
                        print(f"  {_ANSI_GREEN}{label} \u2714 {ex['name']}{_ANSI_RESET}")
                    except Exception as exc:
                        print(f"  {_ANSI_RED}{label} \u2718 {ex['name']}{_ANSI_RESET}")
                state.save()
                print(f"\n  {_progress_bar(state.n_done, state.total)}")
                print(f"\n  {_ANSI_DIM}Press any key to return...{_ANSI_RESET}", flush=True)
                while _read_key_nonblocking() is None:
                    time.sleep(0.05)
                needs_render = True

            if not manual_run and not exercise_passed:
                if _check_file_changed():
                    _do_run()
                    needs_render = True

    except KeyboardInterrupt:
        pass
    finally:
        _restore_terminal()
        print()

    return 0


def cmd_reset(args: argparse.Namespace) -> int:
    config = load_config()
    if args.exercise == "progress":
        if STATE_FILE.exists():
            STATE_FILE.unlink()
            print("progress reset — all exercises marked pending")
        else:
            print("no progress file found")
        return 0
    ex = find_exercise(config, args.exercise)
    if _reset_exercise(ex):
        print(f"reset {ex['name']} — file restored from git")
        if STATE_FILE.exists():
            selected = select_exercises(config, None)
            state = WatchState(selected)
            state.mark_pending(ex["name"])
            print(f"  (marked as pending)")
    else:
        print(f"reset failed for {ex['name']} — git checkout failed", file=sys.stderr)
        return 1
    return 0


def _load_unit_lesson_ranges() -> dict[str, tuple[int, int]]:
    """Parse [[units]] from clings.toml to get unit -> (start, end) lesson ranges."""
    config_path = PKG_DIR / "clings.toml"
    if not config_path.exists():
        return {}
    with open(config_path, "rb") as f:
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

    pkg_exercises = PKG_DIR / "exercises"
    if not pkg_exercises.exists():
        print("error: exercises not found in clings package — reinstall clings",
              file=sys.stderr)
        return 1

    pkg_config = PKG_DIR / "clings.toml"
    target_exercises = ROOT / "exercises"
    target_config = ROOT / "clings.toml"

    # Generate clings.toml with only the requested units (merge if exists)
    if pkg_config.exists():
        with open(pkg_config, "rb") as cf:
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

    all_pkg_exercises = discover_exercises(PKG_DIR)
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


def _next_pending_exercise(config: dict) -> dict | None:
    all_ex = exercises(config)
    if not STATE_FILE.exists():
        return all_ex[0] if all_ex else None
    state = WatchState(all_ex)
    ex = state.current_exercise()
    return None if state.is_done(ex) and state.all_done() else ex


def _get_version() -> str:
    """Read version from pyproject.toml bundled with the package."""
    try:
        with open(PKG_DIR / "pyproject.toml", "rb") as f:
            return tomllib.load(f).get("project", {}).get("version", "unknown")
    except FileNotFoundError:
        return "unknown"


def main() -> int:
    parser = argparse.ArgumentParser(
        prog="clings",
        description="Rustlings-style C exercises. Run without subcommand to enter watch mode.",
    )
    parser.add_argument("-v", "--version", action="version",
                        version=f"%(prog)s {_get_version()}")
    sub = parser.add_subparsers(dest="command")

    p = sub.add_parser("list", help="list exercises with progress status")
    p.add_argument("selector", nargs="?")
    p.set_defaults(func=cmd_list)

    p = sub.add_parser("hint", help="show hint for an exercise (default: next pending)")
    p.add_argument("exercise", nargs="?")
    p.set_defaults(func=cmd_hint)

    p = sub.add_parser("run", help="run an exercise (default: next pending, 'random' for random)")
    p.add_argument("exercise", nargs="?")
    p.add_argument("--solutions", action="store_true")
    p.add_argument("--hidden", action="store_true")
    p.set_defaults(func=cmd_run)

    p = sub.add_parser("check", help="batch verify exercises")
    p.add_argument("selector", nargs="?")
    p.add_argument("--solutions", action="store_true")
    p.add_argument("--hidden", action="store_true")
    p.set_defaults(func=cmd_check)

    p = sub.add_parser("watch", help="interactive watch mode (default when no subcommand)")
    p.add_argument("selector", nargs="?")
    p.add_argument("--solutions", action="store_true")
    p.add_argument("--hidden", action="store_true")
    p.add_argument("--manual-run", action="store_true",
                   help="disable auto-rerun on file change; press r to rerun")
    p.add_argument("--edit-cmd", metavar="CMD",
                   help="command to open exercise file (e.g. 'code' or 'vim')")
    p.set_defaults(func=cmd_watch)

    p = sub.add_parser("reset", help="reset exercise file or all progress")
    p.add_argument("exercise", help="exercise name or 'progress' to reset all progress")
    p.set_defaults(func=cmd_reset)

    p = sub.add_parser("init", help="initialize exercises (default: unit1)")
    available_units = ", ".join(list(UNIT_LESSON_RANGES.keys()) + ["all"])
    p.add_argument("unit", nargs="?", default="unit1",
                   help=f"unit to initialize: {available_units} (default: unit1)")
    p.set_defaults(func=cmd_init)

    p = sub.add_parser("doctor", help="check environment")
    p.set_defaults(func=cmd_doctor)

    args = parser.parse_args()

    if args.command is None:
        args.selector = None
        args.solutions = False
        args.hidden = False
        args.manual_run = False
        args.func = cmd_watch

    try:
        return args.func(args)
    except KeyboardInterrupt:
        return 130
    except ClingsError as exc:
        print(exc, file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
