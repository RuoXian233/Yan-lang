from builtins_py.common import *


def produceAdder(x):
    def add(y):
        nonlocal x
        return x + y

    return add

adder = produceAdder(3)
yan_builtin_impl_println(adder(4))

