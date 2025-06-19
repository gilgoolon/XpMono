from dataclasses import dataclass
import json
import logging
from pathlib import Path
from typing import Dict, List, Optional
import dacite

@dataclass
class Fig:
    name: str
    dll_path: str
    fig_id: int
    version_major: int
    version_minor: int
    operations: dict
    
    @classmethod
    def from_dict(cls, data: dict) -> "Fig":
        result = dacite.from_dict(data_class=Fig, data=data)
        result.fig_id = int(result.fig_id)
        result.operations = {int(key): value for key,
                             value in result.operations.items()}
        return result
    

FIGS_METADATA_PATH = Path(__file__).parent / "figs.json"
_FIGS: Optional[Dict[int, Fig]] = None

RELEASES_PATH = Path("CornCake") / "releases"


def read_metadata() -> None:
    global _FIGS
    if _FIGS is not None:
        return
    data = json.loads(FIGS_METADATA_PATH.read_text())
    figs = [Fig.from_dict(object) for object in data["figs"]]
    _FIGS = {
        fig.fig_id: fig for fig in figs
    }


def list_figs() -> List[Fig]:
    global _FIGS
    read_metadata()
    
    return _FIGS.values()


def get_fig(fig_id: int) -> Fig:
    global _FIGS
    read_metadata()

    return _FIGS[fig_id]


def format_fig_name(fig_id: id) -> str:
    return f"{get_fig(fig_id).name} ({fig_id})"


def format_operation_name(fig_id: int, operation_type: int) -> str:
    global _FIGS
    return get_fig(fig_id).operations[operation_type]
