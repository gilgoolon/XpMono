from functools import wraps


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
