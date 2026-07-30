"""Sponsor FastAPI Router — serves config and static assets."""
import os
from pathlib import Path

from fastapi import APIRouter, HTTPException
from fastapi.responses import FileResponse
from pydantic import BaseModel

router = APIRouter(prefix="/api/sponsor", tags=["sponsor"])

_config = None

STATIC_ASSETS_DIR = Path(__file__).parent.parent / "static" / "assets"


def set_config(config):
    global _config
    _config = config


def get_config():
    return _config


class OpenUrlRequest(BaseModel):
    url: str


@router.post("/open-url")
async def open_url(req: OpenUrlRequest):
    import webbrowser
    try:
        webbrowser.open(req.url)
        return {"success": True}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


@router.get("/config")
async def sponsor_config():
    """返回赞助配置供前端动态渲染"""
    if not _config:
        return {"methods": [], "tutorials": [], "project_name": "", "project_version": ""}
    from dataclasses import asdict
    return asdict(_config)


@router.get("/assets/{filename}")
async def serve_asset(filename: str):
    """提供 sponsor assets 目录中的静态文件"""
    filepath = STATIC_ASSETS_DIR / filename
    if not filepath.is_file():
        raise HTTPException(status_code=404, detail="Asset not found")
    return FileResponse(str(filepath))
