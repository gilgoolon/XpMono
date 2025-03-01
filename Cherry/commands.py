from pathlib import Path


def get_client_commands_dir(root: Path, client_id: str) -> Path:
    return root / client_id
