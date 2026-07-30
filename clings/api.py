"""FastAPI application for Clings."""

import os
import shutil
import sys
import tempfile
import threading
import zipfile
from pathlib import Path
from urllib.request import urlopen

from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse

from .core.compiler import CCompiler
from .core.config import Config
from .core.state import State

GITEE_REPO = "https://gitee.com/lilyco42/clings/repository/archive/main.zip"
PKG_DIR = Path(__file__).parent
CWD = Path.cwd()


def _copy_exercises_from_package():
    """Copy exercises + clings.toml from package to cwd if missing."""
    pkg_exercises = PKG_DIR / "exercises"
    pkg_config = PKG_DIR / "clings.toml"
    cwd_exercises = CWD / "exercises"
    cwd_config = CWD / "clings.toml"

    if not pkg_exercises.exists():
        return

    # copy clings.toml if missing
    if not cwd_config.exists() and pkg_config.exists():
        shutil.copy2(pkg_config, cwd_config)
        print("[clings] created clings.toml")

    # copy exercises if missing
    if not cwd_exercises.exists():
        shutil.copytree(pkg_exercises, cwd_exercises)
        print("[clings] copied exercises to current directory")


def _download_exercises():
    """Download exercises from Gitee if not in package or cwd."""
    pkg_exercises = PKG_DIR / "exercises"
    cwd_exercises = CWD / "exercises"
    if pkg_exercises.exists() or cwd_exercises.exists():
        return  # already have exercises

    print("[clings] downloading exercises from gitee...")
    try:
        with urlopen(GITEE_REPO, timeout=60) as resp:
            data = resp.read()
        with tempfile.NamedTemporaryFile(suffix=".zip", delete=False) as f:
            f.write(data)
            tmp_path = f.name
        with zipfile.ZipFile(tmp_path) as zf:
            names = zf.namelist()
            prefix = names[0].split("/")[0] + "/" if names else ""
            for name in names:
                if "/exercises/" in name or name.endswith("/clings.toml"):
                    rel = name[len(prefix):] if name.startswith(prefix) else name
                    if rel:
                        target = CWD / rel
                        target.parent.mkdir(parents=True, exist_ok=True)
                        if not name.endswith("/"):
                            with zf.open(name) as src, open(target, "wb") as dst:
                                dst.write(src.read())
        os.unlink(tmp_path)
        print("[clings] exercises downloaded!")
    except Exception as e:
        print(f"[clings] download failed: {e}", file=sys.stderr)


# Init on import
_copy_exercises_from_package()
if not (PKG_DIR / "exercises").exists() and not (CWD / "exercises").exists():
    threading.Thread(target=_download_exercises, daemon=True).start()

# Initialize components
compiler = CCompiler()
config = Config()
state = State()

# Create FastAPI app
app = FastAPI(title="Clings API", version="4.10.6")

# Mount static files
static_dir = PKG_DIR / "static"
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

# Sponsor module (embedded)
from .sponsor import sponsor_router, set_config, SponsorConfig, SponsorMethod, TutorialLink

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
    project_version="4.10.6",
    project_repo="https://github.com/lilyco-42/clings",
    project_desc="C 语言交互式练习平台",
)

app.include_router(sponsor_router)
set_config(sponsor_config)
