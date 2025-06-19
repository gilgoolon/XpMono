import sys
import logging
from inspect import stack


_LOGGERS = {}


def get():
    caller_name = stack()[1].frame.f_globals.get("__name__", "default")

    if caller_name in _LOGGERS:
        return _LOGGERS[caller_name]

    logger = logging.getLogger(caller_name)
    logger.setLevel(logging.DEBUG)
    logger.propagate = False  # Avoid duplicate logs if root logger is configured

    formatter = logging.Formatter(
        '[%(asctime)s] [%(levelname)s] [%(name)s]: %(message)s',
        datefmt='%Y-%m-%d %H:%M:%S'
    )

    console_handler = logging.StreamHandler(sys.stdout)
    console_handler.setFormatter(formatter)
    logger.addHandler(console_handler)

    log_filename = f"{caller_name}.log"
    file_handler = logging.FileHandler(log_filename)
    file_handler.setFormatter(formatter)
    logger.addHandler(file_handler)

    _LOGGERS[caller_name] = logger
    return logger
