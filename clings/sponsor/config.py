"""Sponsor configuration."""
from dataclasses import dataclass, field
from typing import List


@dataclass
class TutorialLink:
    """教程链接"""
    title: str
    url: str
    icon: str = "▶"


@dataclass
class SponsorMethod:
    """单个赞助方式"""
    name: str
    icon: str
    url: str = ""
    qr_image: str = ""


@dataclass
class SponsorConfig:
    """赞助配置"""
    methods: List[SponsorMethod] = field(default_factory=list)
    tutorials: List[TutorialLink] = field(default_factory=list)
    project_name: str = "Clings"
    project_version: str = "1.0.0"
    project_repo: str = ""
    project_desc: str = ""
