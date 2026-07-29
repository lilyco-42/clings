"""FastAPI application for Clings."""

from pathlib import Path
from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse

from .core.compiler import CCompiler
from .core.config import Config
from .core.state import State

# Initialize components
compiler = CCompiler()
config = Config()
state = State()

# Create FastAPI app
app = FastAPI(title="Clings API", version="4.8.0")

# Mount static files
static_dir = Path(__file__).parent / "static"
if static_dir.exists():
    app.mount("/static", StaticFiles(directory=str(static_dir)), name="static")


@app.get("/")
async def index():
    """Serve the main HTML page."""
    index_file = static_dir / "index.html"
    if index_file.exists():
        return FileResponse(str(index_file))
    return {"message": "Clings API is running", "docs": "/docs"}


@app.get("/api/health")
async def health():
    """Health check endpoint."""
    return {
        "status": "ok",
        "compiler": compiler.compiler,
        "compiler_available": compiler.available,
    }


# Include routers
from .routes import exercises, compile, progress

app.include_router(exercises.router, prefix="/api", tags=["exercises"])
app.include_router(compile.router, prefix="/api", tags=["compile"])
app.include_router(progress.router, prefix="/api", tags=["progress"])
