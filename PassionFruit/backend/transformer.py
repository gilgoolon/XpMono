
import abc
from functools import cache
from typing import List

from PassionFruit.backend.transformers.releases_embedder import ReleasesEmbedderTransformer
from PoopBiter import command_type


class CommandTransformer(abc.ABC):
    @abc.abstractmethod
    def transform(self, command_data: dict) -> None:
        pass

    @classmethod
    def _get_command_type(cls, command_data: dict) -> command_type.Type:
        return command_type.Type(command_data["info"]["type"])

    @classmethod
    def _get_command_parameters(cls, command_data: dict) -> dict:
        return command_data["parameters"]



@cache
def get_transformers() -> List[CommandTransformer]:
    return [ReleasesEmbedderTransformer]


def transform_command(command_data: dict) -> None:
    for transformer in get_transformers():
        transformer.transform(command_data)
