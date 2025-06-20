import os
from pathlib import Path
from typing import List


TEMPLATES_FOLDER = Path("CornCake") / "templates"


def list_templates() -> List[Path]:
    return {TEMPLATES_FOLDER / file for file in os.listdir(TEMPLATES_FOLDER) if file.endswith(".json")}
