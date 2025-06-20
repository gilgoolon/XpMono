
from functools import cache
from typing import List

from PassionFruit.backend.transformers.releases_embedder import ReleasesEmbedderTransformer
from PassionFruit.backend.transformers.transformer import CommandTransformer


@cache
def get_transformers() -> List[CommandTransformer]:
    return [ReleasesEmbedderTransformer()]


def transform_command(command_data: dict) -> None:
    for transformer in get_transformers():
        transformer.transform(command_data)
