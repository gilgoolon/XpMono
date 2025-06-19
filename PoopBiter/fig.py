from dataclasses import dataclass
import json
from pathlib import Path
from typing import Dict, List, Optional
from dacite import from_dict

@dataclass
class Fig:
    name: str
    dll_path: Path
    fig_id: int
    version_major: int
    version_minor: int
    operations: Dict[int, str]
    
    @classmethod
    def from_dict(cls, data: dict) -> "Fig":
        return from_dict(data_class=Fig, data=data)
    

FIGS_METADATA_PATH = Path(__file__).parent / "figs.json"
_FIGS: Optional[Dict[int, Fig]] = None

RELEASES_PATH = Path("CornCake") / "releases"


def read_metadata() -> None:
    if _FIGS is None:
        return
    data = json.loads(FIGS_METADATA_PATH.read_text())
    figs = [Fig.from_dict(object) for object in data["figs"]]
    _FIGS = {
        fig.fig_id: fig for fig in figs
    }


def list_figs() -> List[Fig]:
    read_metadata()
    
    return _FIGS.values()


def get_fig(fig_id: int) -> Fig:
    read_metadata()

    return _FIGS[fig_id]


def format_operation_name(fig_id: int, operation_type: int) -> str:
    return get_fig(fig_id).operations[operation_type]
