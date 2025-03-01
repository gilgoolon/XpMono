from pathlib import Path


def get_client_commands_dir(root: Path, client_id: int) -> Path:
    return root / f"{client_id:x}"
