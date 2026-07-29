"""Exercise progress state management."""

import json
from pathlib import Path
from dataclasses import dataclass, asdict
from datetime import datetime


@dataclass
class ExerciseState:
    name: str
    status: str = "pending"  # pending, passed, failed
    attempts: int = 0
    last_attempt: str | None = None
    error: str | None = None


class State:
    """Exercise progress manager."""

    def __init__(self, root: Path | None = None):
        self.root = root or Path.cwd()
        self.state_file = self.root / ".clings-state.json"
        self._states: dict[str, ExerciseState] = {}
        self._load()

    def _load(self):
        """Load state from file."""
        if not self.state_file.exists():
            return

        try:
            data = json.loads(self.state_file.read_text(encoding="utf-8"))
            for name, state in data.get("exercises", {}).items():
                self._states[name] = ExerciseState(**state)
        except Exception:
            pass

    def _save(self):
        """Save state to file."""
        data = {
            "exercises": {name: asdict(state) for name, state in self._states.items()}
        }
        self.state_file.write_text(
            json.dumps(data, indent=2, ensure_ascii=False), encoding="utf-8"
        )

    def get_state(self, name: str) -> ExerciseState:
        """Get state for an exercise."""
        if name not in self._states:
            self._states[name] = ExerciseState(name=name)
        return self._states[name]

    def mark_passed(self, name: str):
        """Mark exercise as passed."""
        state = self.get_state(name)
        state.status = "passed"
        state.attempts += 1
        state.last_attempt = datetime.now().isoformat()
        state.error = None
        self._save()

    def mark_failed(self, name: str, error: str = ""):
        """Mark exercise as failed."""
        state = self.get_state(name)
        state.status = "failed"
        state.attempts += 1
        state.last_attempt = datetime.now().isoformat()
        state.error = error
        self._save()

    def get_progress(self) -> dict:
        """Get overall progress."""
        total = len(self._states)
        passed = sum(1 for s in self._states.values() if s.status == "passed")
        return {
            "total": total,
            "passed": passed,
            "failed": total - passed,
            "percentage": (passed / total * 100) if total > 0 else 0,
        }

    def get_all_states(self) -> dict[str, dict]:
        """Get all exercise states."""
        return {name: asdict(state) for name, state in self._states.items()}

    def reset(self, name: str | None = None):
        """Reset exercise state."""
        if name:
            self._states.pop(name, None)
        else:
            self._states.clear()
        self._save()

    def is_passed(self, name: str) -> bool:
        """Check if exercise is passed."""
        return self.get_state(name).status == "passed"
