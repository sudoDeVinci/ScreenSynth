from gc import collect
from custom_typing import Generator
from micropython import const
import ustruct

FILENAME = "wave.wav"

# If debug is True, our debug lines throughtout the code will print. Otherwise, do nothing
DEBUG:bool = True
def out01(x:str) -> None:
    pass
def out02(x:str) -> None:
    print(x)
debug = out02 if DEBUG else out01

def getsizeof(obj: object) -> int:
    """
    Calculate the approximate memory size of an object in bytes.

    This function uses the garbage collector to clean up memory before
    calculating the size. It handles various types of objects including
    primitive types, collections, and custom objects.

    :param obj: The object whose size is to be calculated.
    :return: The size of the object in bytes.
    """
    collect()
    if isinstance(obj, (int, float, bool)):
        return ustruct.calcsize('P')  # Size of a pointer
    elif isinstance(obj, (str, bytes, bytearray)):
        return len(obj)
    elif isinstance(obj, (list, tuple, set, frozenset)):
        return sum(getsizeof(item) for item in obj) + ustruct.calcsize('P') * len(obj)
    elif isinstance(obj, dict):
        return sum(getsizeof(k) + getsizeof(v) for k, v in obj.items()) + ustruct.calcsize('P') * len(obj)
    else:
        return ustruct.calcsize('P') + getsizeof(obj.__dict__)
