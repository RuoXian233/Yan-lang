from typing import Any, Sequence
import math
import inspect


class Panic(Exception):
    pass


def yan_builtin_impl_println(arg: Any) -> None:
    print(arg)

def yan_builtin_impl_print(arg: Any) -> None:
    print(arg, end='')
    
def yan_builtin_impl_readLine() -> str:
    return input()

def yan_builtin_impl_typeof(o: Any) -> str:
    return str(type(o))

def yan_builtin_impl_len(s: Sequence[Any]) -> int:
    return len(s)

def yan_builtin_impl_range(start: int, end: int, step: int = 1) -> range:
    return range(start, end, step)

def yan_builtin_impl_parseInt(s: str) -> int:
    return int(s)

def yan_builtin_impl_parseFloat(s: str) -> float:
    return float(s)

def yan_pyeval(code: str) -> Any:
    exec(code, globals(), inspect.currentframe().f_back.f_locals) # type: ignore

def yan_builtin_impl_str(o: Any) -> str:
    return str(o)

def yan_builtin_impl_panic(o: Any) -> str:
    raise Panic(str(o))

def yan_builtin_impl_eval(code: str) -> None:
    eval(code)

def yan_builtin_impl_sin(x: int | float) -> float:
    return math.sin(x)

def yan_builtin_impl_cos(x: int | float) -> float:
    return math.cos(x)

def yan_builtin_impl_tan(x: int | float) -> float:
    return math.tan(x)  

def yan_builtin_impl_log(x: int | float) -> float:
    return math.log(x)

def yan_builtin_impl_ln(x: int | float) -> float:
    return math.log(x, math.e)

def yan_builtin_impl_sqrt(x: int | float) -> float:
    return math.sqrt(x)

def yan_builtin_impl_abs(n: float | int) -> float | int:
    return abs(n)

def yan_builtin_impl_set(l: list[Any], n: int, v: Any) -> None:
    l[n] = v

def yan_builtin_impl_input(prompt: str = '') -> str:
    return input(prompt)

def yan_builtin_impl_append(l: list[Any], v: Any) -> None:
    l.append(v)

def yan_builtin_impl_concat(l: list[Any], v: list[Any]) -> None:
    l.extend(v)

def yan_builitin_impl_remove(l: list[Any], i: int) -> None:
    del l[i]

def yan_builtin_impl_addressOf(o: Any) -> Any:
    return id(o)

def yan_builtin_impl_del(o: str) -> None:
    raise Exception(f'Unsupported operation in python support: del {o}')

def yan_builtin_impl_recover(x: Any = None) -> None:
    raise Exception(f'Unsupported operation in python support: recover()')

def yan_builtin_impl_builtins() -> list[str]:
    return []

def yan_builtin_impl_keys(d: dict) -> Any:
    return d.keys()

def yan_builtin_impl_values(d: dict) -> Any:
    return d.values()

def yan_builtin_impl_isInteger(o: Any) -> Any:
    return isinstance(o, int)

def yan_builtin_impl_isFloating(o: Any) -> Any:
    return isinstance(o, float)


class YanLibs:
    import random
    import os
    import time

    BUILTIN_MODULES = ('string', 'fs', 'os', 'rand', 'time', 'inspect')

    @staticmethod
    def _impl_yan_rand_random() -> float:
        return YanLibs.random.random()
    
    @staticmethod
    def _impl_yan_randint(a: int, b: int) -> int:
        return YanLibs.random.randint(a, b)
    
    @staticmethod
    def _impl_os_system(cmd: str) -> int:
        return YanLibs.os.system(cmd)
    
    class FileObject:
        
        def __init__(self, name: str, mode: str = 'r') -> None:
            self._file = open(mode)
            self.name = name

        def read(self) -> str:
            return self._file.read()
        
        def write(self, s: str) -> None:
            self._file.write(s)

        def readLines(self) -> list[str]:
            return self._file.readlines()

        def readBuf(self, size: int = 1) -> str:
            raise NotImplemented

        def length(self) -> int:
            return YanLibs.os.path.getsize(self._file.name)

        def isEof(self) -> bool:
            return self._file.tell() == self.length()

    @staticmethod
    def _impl_fs_close(fo: FileObject) -> None:
        fo._file.close()

    @staticmethod
    def _impl_fs_open(path: str, mode: str = 'r') -> FileObject:
        return YanLibs.FileObject(path, mode)
    
    @staticmethod
    def _impl_fs_exits(path: str) -> bool:
        return YanLibs.os.path.exists(path)

    @staticmethod
    def _impl_fs_getFreeSpace(path: str) -> int:
        return YanLibs.os.statvfs(path).f_bfree * YanLibs.os.statvfs(path).f_frsize

    @staticmethod
    def _impl_fs_getFilePermissions(path: str) -> str:
        return str(YanLibs.os.stat(path).st_mode)
    
    @staticmethod
    def _impl_fs_getFileType(path: str) -> str:
        return YanLibs.os.path.isfile(path) and 'file' or 'directory'
    
    @staticmethod
    def _impl_fs_getFileSize(path: str) -> int:
        return YanLibs.os.path.getsize(path)
    
    @staticmethod
    def _impl_fs_listDirectory(path: str) -> list[str]:
        return YanLibs.os.listdir(path)
    
    @staticmethod
    def _impl_fs_getLastWriteTime(path: str) -> str:
        return YanLibs.time.ctime(YanLibs.os.path.getmtime(path))

    @staticmethod
    def _impl_fs_getHardLinksCount(path: str) -> int:
        return YanLibs.os.stat(path).st_nlink

    # TODO: impl more modules

def yan_builtin_impl_require(module: str) -> None:
    if module.startswith('@'):
        raise Exception(f'Unsupported operation in python support: require native module')

    if module in BUILTIN_MODULES:
        # TODO: import builtin modules
        pass    
    else:
        # Execute from $mod import *
        mod = __import__(module)
        for attr in dir(module):
            globals()[attr] = getattr(mod, attr)


def yan_builtin_impl_import(module: str) -> Any:
    if module.startswith('@'): 
        raise Exception(f'Unsupported operation in python support: import native module')
    
    if len(module.split('.')) > 2:
        raise Exception(f'Invilid import specification')
    
    moduleFile = module.split('.')[0]
    moduleAttr = module.split('.')[1]


null = 0


class yan_dict_impl(dict):
    
    def __getattr__(self, name: str):
        return self[name]
    
    def __setattr__(self, name: str, value: Any):
        self[name] = value

    def __call__(self, *args, **kwargs):
        if '__init__' in self and '__cls__' in self:
            self['__init__'](self)
            return self
        else:
            raise Exception(f'Unsupported operation in python support: new {self}')
