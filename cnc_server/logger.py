import logging
from pathlib import Path
import sys


def make_logger(path: Path, name: str = "Server Logger", log_format: str = "%(asctime)s - %(levelname)s - %(message)s") -> logging.Logger:
    logger = logging.getLogger(name)
    logger.setLevel(logging.DEBUG)

    formatter = logging.Formatter(log_format)

    file_handler = logging.FileHandler(path.as_posix())
    file_handler.setFormatter(formatter)
    file_handler.setLevel(logging.DEBUG)

    console_handler = logging.StreamHandler(sys.stdout)
    console_handler.setFormatter(formatter)
    console_handler.setLevel(logging.INFO)

    logger.addHandler(file_handler)
    logger.addHandler(console_handler)

    return logger
