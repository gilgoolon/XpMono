import os
from pathlib import Path
from typing import Dict, List, Optional


RELEASES_PATH = Path("CornCake") / "releases"


def list_releases() -> Dict[str, Path]:
    return {file: RELEASES_PATH / file for file in os.listdir(RELEASES_PATH)} if RELEASES_PATH.exists() and RELEASES_PATH.is_dir() else []


def get_release_path(release_name: str, suffix: Optional[str] = '.dll') -> Path:
    full_name = release_name + suffix if suffix is not None else release_name
    return RELEASES_PATH / full_name
