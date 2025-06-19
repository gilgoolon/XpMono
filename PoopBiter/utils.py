import re
import json
import ctypes
from datetime import datetime
from typing import Any, Optional


def unhex(string: str) -> int:
    return int(string, 16)


class SingletonMeta(type):
    _instances = {}
    _init_args = {}

    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._init_args[cls] = (args, kwargs)
            instance = super().__call__(*args, **kwargs)
            cls._instances[cls] = instance
        else:
            prev_args, prev_kwargs = cls._init_args[cls]
            if args or kwargs:
                if args != prev_args or kwargs != prev_kwargs:
                    raise TypeError(
                        f"{cls.__name__} singleton already initialized with different arguments")
        return cls._instances[cls]


def now_filename(prefix: str = "", extension: Optional[str] = "log") -> str:
    now = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    name = f"{prefix}_{now}.{extension}"
    return name if extension is None else f"{name}.{extension}"


def dump_pretty_json(object: Any) -> str:
    return json.dumps(object, indent=4)


def format_exception(ex: BaseException) -> str:
    return f"{ex.__class__.__name__}(\"{ex}\")"


def is_int(value: Any) -> bool:
    try:
        int(value)
        return True
    except ValueError:
        return False


def to_snake_case(camel_case_word: str) -> str:
    return re.sub(r'(?<!^)(?=[A-Z])', '_', camel_case_word).lower()


def format_winapi_error(error_code: int) -> str:
    FORMAT_MESSAGE_ALLOCATE_BUFFER = 0x00000100
    FORMAT_MESSAGE_FROM_SYSTEM = 0x00001000
    FORMAT_MESSAGE_IGNORE_INSERTS = 0x00000200
    FORMAT_MESSAGE_DEFAULT_LANGUAGE = 0x0

    SOURCE_BUFFER_IGNORED = None
    BUFFER_SIZE_IGNORED = 0
    VARIABLE_ARGUMENTS_IGNORED = None

    result_buffer = ctypes.c_wchar_p()

    result = ctypes.windll.kernel32.FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        SOURCE_BUFFER_IGNORED,
        error_code,
        FORMAT_MESSAGE_DEFAULT_LANGUAGE,
        ctypes.byref(result_buffer),
        BUFFER_SIZE_IGNORED,
        VARIABLE_ARGUMENTS_IGNORED
    )

    FAILURE = 0
    if result == FAILURE or not result_buffer:
        raise ValueError(f"failed to format winapi error code: {error_code}")

    message = ctypes.wstring_at(result_buffer).strip()

    ctypes.windll.kernel32.LocalFree(result_buffer)

    return message
