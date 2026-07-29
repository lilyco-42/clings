"""Progress routes."""

from fastapi import APIRouter
from clings.api import state

router = APIRouter()


@router.get("/progress")
async def get_progress():
    """Get overall progress."""
    return state.get_progress()


@router.get("/progress/states")
async def get_all_states():
    """Get all exercise states."""
    return state.get_all_states()


@router.get("/progress/{name}")
async def get_exercise_state(name: str):
    """Get state for a specific exercise."""
    exercise_state = state.get_state(name)
    return {
        "name": exercise_state.name,
        "status": exercise_state.status,
        "attempts": exercise_state.attempts,
        "last_attempt": exercise_state.last_attempt,
        "error": exercise_state.error,
    }


@router.post("/progress/{name}/reset")
async def reset_exercise(name: str):
    """Reset exercise state."""
    state.reset(name)
    return {"message": f"Exercise '{name}' state reset"}


@router.post("/progress/reset")
async def reset_all():
    """Reset all progress."""
    state.reset()
    return {"message": "All progress reset"}
