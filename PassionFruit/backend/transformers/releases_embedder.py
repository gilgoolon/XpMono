
import os
from pathlib import Path
from typing import Union

from PoopBiter import command_type
from PassionFruit.backend.transformer import CommandTransformer


class ReleasesEmbedderTransformer(CommandTransformer):
    def __init__(self, releases_folder: Path) -> None:
        super().__init__()
        self._releases = {
            release: releases_folder / release for release in os.listdir(releases_folder)
        }
    
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
        return self._get_command_type(command_data) == command_type.Type.LOAD_FIG

    def _get_release_value(self, release_name: str) -> bytes:
        return self._releases[release_name].read_bytes()
