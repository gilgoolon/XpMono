
from base64 import b64encode
import json
import os

from PoopBiter import command_type, releases
from PassionFruit.backend.transformers.transformer import CommandTransformer


class GoogleApiKeyEmbedderTransformer(CommandTransformer):
    def __init__(self) -> None:
        super().__init__()
        self._api_key = os.getenv("GOOGLE_LOCATION_API_KEY")

    def transform(self, command_data: dict) -> None:
        if not self._should_transform(command_data):
            return

        PARAMETERS_NAME = "parameters"
        parameters = self._get_command_parameters(command_data)
        if PARAMETERS_NAME in parameters:
            parameters_parameter = parameters.get(PARAMETERS_NAME, "")
            parameters_parameter = {} if len(
                parameters_parameter) == 0 else json.loads(parameters_parameter)
            parameters_parameter["google_api_key"] = f"\"{self._api_key}\""
            parameters[PARAMETERS_NAME] = json.dumps(parameters_parameter)

    def _should_transform(self, command_data: dict) -> bool:
        return self._get_command_type(command_data) == command_type.Type.EXECUTE_FIG_OPERATION
