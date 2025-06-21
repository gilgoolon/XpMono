
from base64 import b64encode
from PoopBiter import command_type
from PassionFruit.backend.transformers.transformer import CommandTransformer


class ParametersEncoderTransformer(CommandTransformer):
    def transform(self, command_data: dict) -> None:
        if not self._should_transform(command_data):
            return

        parameters = self._get_command_parameters(command_data)
        parameters_parameter = parameters.get("parameters", None)
        if parameters_parameter is None or not isinstance(parameters_parameter, str):
            return
        
        parameters["parameters"] = b64encode(parameters_parameter.encode()).decode()

    def _should_transform(self, command_data: dict) -> bool:
        return self._get_command_type(command_data) in [command_type.Type.CALL_DLL_GENERIC_PROCEDURE, command_type.Type.EXECUTE_FIG_OPERATION]
