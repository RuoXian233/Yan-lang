from typing import Any


def yan_builtin_impl_println(arg: Any) -> None:
    print(arg)

def yan_builtin_impl_print(arg: Any) -> None:
    print(arg, end='')
    