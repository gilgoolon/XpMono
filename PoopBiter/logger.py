import sys
import logging
import requests
import __main__
import urllib.parse
from pathlib import Path
from typing import Optional

from PoopBiter.utils import dump_pretty_json, now_filename


_LOGS_FOLDER = Path("logs")
_API_LOGS_FOLDER = _LOGS_FOLDER / "apis"
_LOGGER = None


class ExceptionContextFilter(logging.Filter):
    def filter(self, record: logging.LogRecord) -> bool:
        # If exc_info wasn't passed but we're inside an exception, attach the exception info
        if record.exc_info is None:
            exc_type, exc_value, exc_tb = sys.exc_info()
            if exc_type is not None:
                record.exc_info = (exc_type, exc_value, exc_tb)
        return True


def get():
    global _LOGGER
    if _LOGGER:
        return _LOGGER

    logger_name = getattr(__main__, '__package__', None) or getattr(
        __main__, '__name__', 'default')

    logger = logging.getLogger(logger_name)
    logger.setLevel(logging.DEBUG)
    logger.propagate = False

    formatter = logging.Formatter(
        '[%(asctime)s] [%(levelname)s] [%(name)s]: %(message)s',
        datefmt='%Y-%m-%d %H:%M:%S'
    )

    console_handler = logging.StreamHandler(sys.stdout)
    console_handler.setFormatter(formatter)
    console_handler.setLevel(logging.INFO)
    logger.addHandler(console_handler)

    log_path = _LOGS_FOLDER / f"{logger_name}.log"
    log_path.parent.mkdir(exist_ok=True)

    file_handler = logging.FileHandler(log_path)
    file_handler.setFormatter(formatter)
    file_handler.addFilter(ExceptionContextFilter())
    file_handler.setLevel(logging.DEBUG)
    logger.addHandler(file_handler)

    _LOGGER = logger
    return logger


def debug(*args, **kwargs):
    return get().debug(*args, **kwargs)


def info(*args, **kwargs):
    return get().info(*args, **kwargs)


def warning(*args, **kwargs):
    return get().warning(*args, **kwargs)


def error(*args, **kwargs):
    return get().error(*args, **kwargs)


def critical(*args, **kwargs):
    return get().critical(*args, **kwargs)


def api(url: str, request_data: Optional[dict], response: requests.Response) -> None:
    parsed_url = urllib.parse.urlsplit(url)
    endpoint = parsed_url.netloc + parsed_url.path
    query = urllib.parse.parse_qs(parsed_url.query)

    info(f"api call to '{endpoint}' returned {response.status_code}")

    dump_path = _API_LOGS_FOLDER / endpoint / now_filename()
    dump_path.parent.mkdir(parents=True, exist_ok=True)

    content_type = response.headers["content-type"]

    dump = {
        "endpoint": endpoint,
        "url": url,
        "query": query,
        "request_data": request_data,
        "response": {
            "code": response.status_code,
            "encoding": response.encoding,
            "content_type": content_type,
            "content": response.json() if "json" in content_type else repr(response.text)
        }
    }

    dump_path.write_text(dump_pretty_json(dump))

    debug(f"api call dumped verbosely to {dump_path}")
