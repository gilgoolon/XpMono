
import os
from pathlib import Path

from PoopBiter import command_type, logger, releases
from PassionFruit.backend.transformers.transformer import CommandTransformer


class ReleasesEmbedderTransformer(CommandTransformer):
    def __init__(self) -> None:
        super().__init__()
        self._releases = releases.list_releases()
    
    def transform(self, command_data: dict) -> None:
        if not self._should_transform(command_data):
            return
        
        parameters = self._get_command_parameters(command_data)
        for name, value in parameters.items():
            if not isinstance(value, dict):
                continue
            
            RELEASE_OBJECT_TYPE = "release"
            if value["type"] != RELEASE_OBJECT_TYPE:
                continue
            
            release = value["value"]
            new_value = self._get_release_value(release)
            parameters[name] = new_value
    
    def _should_transform(self, command_data: dict) -> bool:
        logger.debug(command_data)
        return self._get_command_type(command_data) == command_type.Type.LOAD_FIG

    def _get_release_value(self, release_name: str) -> bytes:
        return self._releases[release_name].read_bytes()
