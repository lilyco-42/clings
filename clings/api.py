"""FastAPI application for Clings."""

import sys
import os
from pathlib import Path
from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse

from .core.compiler import CCompiler
from .core.config import Config
from .core.state import State

# Add shared module to path
_shared_dir = Path(__file__).parent.parent / "shared"
if _shared_dir.exists():
    sys.path.insert(0, str(_shared_dir))

# Initialize components
compiler = CCompiler()
config = Config()
state = State()

# Create FastAPI app
app = FastAPI(title="Clings API", version="4.10.0")

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

# Sponsor module
try:
    from sponsor import (
        sponsor_router,
        set_config,
        SponsorConfig,
        SponsorMethod,
        TutorialLink,
    )

    sponsor_config = SponsorConfig(
        methods=[
            SponsorMethod(name="微信支付", icon="💚", qr_image="assets/wechatpay.png"),
            SponsorMethod(name="支付宝", icon="💙", qr_image="assets/alipay.png"),
            SponsorMethod(name="GitHub Sponsors", icon="💛", url="https://github.com/sponsors/lilyco-42"),
        ],
        tutorials=[
            TutorialLink(title="课程主页", icon="📚", url="https://opencamp.cn/C/camp/2026"),
            TutorialLink(title="GitHub 仓库", icon="📦", url="https://github.com/lilyco-42/clings"),
        ],
        project_name="Clings",
        project_version="4.9.0",
        project_repo="https://github.com/lilyco-42/clings",
        project_desc="C 语言交互式练习平台",
    )

    app.include_router(sponsor_router)
    set_config(sponsor_config)
except ImportError:
    pass
