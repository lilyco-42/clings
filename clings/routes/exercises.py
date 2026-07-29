"""Exercise routes."""

from fastapi import APIRouter, HTTPException
from clings.api import config

router = APIRouter()


@router.get("/exercises")
async def list_exercises(unit: str | None = None):
    """List all exercises, optionally filtered by unit."""
    return {"exercises": config.get_exercises(unit)}


@router.get("/exercises/{name}")
async def get_exercise(name: str):
    """Get exercise details."""
    exercise = config.get_exercise(name)
    if not exercise:
        raise HTTPException(status_code=404, detail=f"Exercise '{name}' not found")
    return exercise


@router.get("/exercises/{name}/source")
async def get_source(name: str):
    """Get exercise source code."""
    source = config.get_source(name)
    if not source:
        raise HTTPException(status_code=404, detail=f"Source for '{name}' not found")
    return {"name": name, "source": source}


@router.get("/exercises/{name}/hint")
async def get_hint(name: str):
    """Get exercise hint."""
    hint = config.get_hint(name)
    return {"name": name, "hint": hint}


@router.get("/units")
async def list_units():
    """List all units."""
    return {"units": config.get_units()}
