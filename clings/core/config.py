"""Exercise configuration and discovery."""

import shutil
import tomllib
from pathlib import Path
from dataclasses import dataclass


@dataclass
class TestCase:
    stdin: str
    stdout: str


@dataclass
class Exercise:
    name: str
    title: str
    unit: str
    lesson: int
    order: int
    mode: str
    source: str
    expected_return: int | None = None
    hint: str = ""
    path: str = ""
    cases: list[TestCase] | None = None


class Config:
    """Exercise configuration manager."""

    def __init__(self, root: Path | None = None):
        pkg_dir = Path(__file__).parent.parent  # clings package dir
        self.root = root or Path.cwd()
        # If exercises missing in cwd, copy from package
        if not (self.root / "exercises").exists() and not (self.root / "clings.toml").exists():
            pkg_exercises = pkg_dir / "exercises"
            pkg_config = pkg_dir / "clings.toml"
            if pkg_exercises.exists():
                try:
                    if not (self.root / "clings.toml").exists() and pkg_config.exists():
                        shutil.copy2(pkg_config, self.root / "clings.toml")
                    if not (self.root / "exercises").exists():
                        shutil.copytree(pkg_exercises, self.root / "exercises")
                except Exception:
                    pass  # fallback to package dir below
        # If still no exercises, use package dir
        if not (self.root / "exercises").exists() and not (self.root / "clings.toml").exists():
            self.root = pkg_dir
        self.exercises_dir = self.root / "exercises"
        self.config_file = self.root / "clings.toml"
        self._exercises: list[Exercise] = []
        self._load()

    def _load(self):
        """Load exercises from config and filesystem."""
        if not self.config_file.exists():
            return

        with open(self.config_file, "rb") as f:
            config = tomllib.load(f)

        units = config.get("units", [])
        for unit in units:
            unit_id = unit.get("id", "")
            lessons = unit.get("lessons", "")
            if "-" in lessons:
                start, end = map(int, lessons.split("-"))
                for lesson_num in range(start, end + 1):
                    self._discover_lessons(unit_id, lesson_num)

    def _discover_lessons(self, unit: str, lesson: int):
        """Discover exercises in a lesson directory."""
        if not self.exercises_dir.exists():
            return

        for ex_dir in sorted(self.exercises_dir.iterdir()):
            if not ex_dir.is_dir():
                continue

            toml_file = ex_dir / "exercises.toml"
            if not toml_file.exists():
                continue

            with open(toml_file, "rb") as f:
                data = tomllib.load(f)

            for ex_data in data.get("exercises", []):
                if ex_data.get("unit") == unit and ex_data.get("lesson") == lesson:
                    cases = None
                    raw_cases = ex_data.get("cases")
                    if raw_cases:
                        cases = [
                            TestCase(stdin=c.get("stdin", ""), stdout=c.get("stdout", ""))
                            for c in raw_cases
                        ]
                    ex = Exercise(
                        name=ex_data.get("name", ""),
                        title=ex_data.get("title", ""),
                        unit=unit,
                        lesson=lesson,
                        order=ex_data.get("order", 0),
                        mode=ex_data.get("mode", "stdout"),
                        source=ex_data.get("source", ""),
                        expected_return=ex_data.get("expected_return"),
                        hint=ex_data.get("hint", ""),
                        path=str(ex_dir.relative_to(self.root)),
                        cases=cases,
                    )
                    self._exercises.append(ex)

    def get_exercises(self, unit: str | None = None) -> list[dict]:
        """Get all exercises, optionally filtered by unit."""
        exercises = self._exercises
        if unit:
            exercises = [e for e in exercises if e.unit == unit]
        return [
            {
                "name": e.name,
                "title": e.title,
                "unit": e.unit,
                "lesson": e.lesson,
                "order": e.order,
                "mode": e.mode,
                "source": e.source,
                "path": e.path,
            }
            for e in exercises
        ]

    def get_exercise(self, name: str) -> dict | None:
        """Get a single exercise by name."""
        for e in self._exercises:
            if e.name == name:
                return {
                    "name": e.name,
                    "title": e.title,
                    "unit": e.unit,
                    "lesson": e.lesson,
                    "order": e.order,
                    "mode": e.mode,
                    "source": e.source,
                    "expected_return": e.expected_return,
                    "hint": e.hint,
                    "path": e.path,
                    "cases": [
                        {"stdin": c.stdin, "stdout": c.stdout}
                        for c in e.cases
                    ] if e.cases else [],
                }
        return None

    def get_hint(self, name: str) -> str:
        """Get hint for an exercise."""
        ex = self.get_exercise(name)
        return ex["hint"] if ex else ""

    def get_source(self, name: str) -> str:
        """Get source code for an exercise."""
        ex = self.get_exercise(name)
        if not ex:
            return ""

        src_path = self.root / ex["path"] / ex["source"]
        if src_path.exists():
            return src_path.read_text(encoding="utf-8")
        return ""

    def get_units(self) -> list[dict]:
        """Get all units."""
        if not self.config_file.exists():
            return []

        with open(self.config_file, "rb") as f:
            config = tomllib.load(f)

        return config.get("units", [])
