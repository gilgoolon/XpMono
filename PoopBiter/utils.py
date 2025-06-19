from datetime import datetime
from functools import wraps
import json
import re
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
