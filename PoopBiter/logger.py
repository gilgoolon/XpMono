import sys
import logging
import __main__


_LOGGER = None


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

    log_filename = f"{logger_name}.log"
    file_handler = logging.FileHandler(log_filename)
    file_handler.setFormatter(formatter)
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
