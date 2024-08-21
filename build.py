import sys
import os


INTERPRETER_SRC = 'yan-main.cpp'
INTERPRETER_BIN = 'yan'
MODULES = ['rand', 'fs', 'string', 'inspect', 'os']
CC_LINUX = 'g++'
CC_WIN32 = 'x86_64-w64-mingw32-g++'
CXX_STANDARD = 'c++20'
FLAGS_INTERPRETER = ''
FLAGS_LIBS = '-shared -fPIC -rdynamic -ldl'
FLAGS_LIBS_WIN32 = '-shared -fPIC'
FLAGS_INTERPRETER_WIN32 = ''
DEBUG = '-g'
DYLIB_PATH = 'lib/'

print('----- Yan Interpreter Maker V1 -----\n')

if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == 'configure':
        if sys.argv[2] == '-linux':
            with open('build.sh', 'w') as f:
                f.write('#!/bin/bash\n')
                total = 1 + len(MODULES)
                f.write(f'echo \'Building interpreter ... (1 / {total}) -> {INTERPRETER_BIN}\'\n')
                # f.write(f'echo \'  {CC} -std={CXX_STANDARD} {DEBUG} {INTERPRETER_SRC} {FLAGS_MAIN} -o lib{INTERPRETER_BIN}.so\'\n')
                # f.write(f'{CC} -std={CXX_STANDARD} {DEBUG} {INTERPRETER_SRC} {FLAGS_MAIN} -o lib{INTERPRETER_BIN}.so\n')
                f.write(f'echo \'  {CC_LINUX} -std={CXX_STANDARD} {DEBUG} {INTERPRETER_SRC} {FLAGS_INTERPRETER} -o {INTERPRETER_BIN}\'\n')
                f.write(f'{CC_LINUX} -std={CXX_STANDARD} {DEBUG} {INTERPRETER_SRC} {FLAGS_INTERPRETER} -o {INTERPRETER_BIN}\n')

                for index, module in enumerate(MODULES):
                    f.write(f'echo \'Building library "{module}" ... ({index + 2} / {total}) -> yan-{module}.so\'\n')
                    f.write(f'echo \'  {CC_LINUX} -std={CXX_STANDARD} {DEBUG} yan-lib{module}.cpp {FLAGS_LIBS} -o {DYLIB_PATH}/yan-{module}.so\'\n')
                    f.write(f'{CC_LINUX} -std={CXX_STANDARD} {DEBUG} yan-lib{module}.cpp {FLAGS_LIBS} -o {DYLIB_PATH}/yan-{module}.so\n')
            os.system('chmod +x build.sh')        
        elif sys.argv[2] == '-win32':
            with open('build-win32.sh', 'w') as f:
                f.write('#!/bin/bash\n')
                total = 1 + len(MODULES)
                f.write(f'echo \'Building interpreter ... (1 / {total}) -> {INTERPRETER_BIN}\'\n')
                # f.write(f'echo \'  {CC} -std={CXX_STANDARD} {DEBUG} {INTERPRETER_SRC} {FLAGS_MAIN} -o lib{INTERPRETER_BIN}.so\'\n')
                # f.write(f'{CC} -std={CXX_STANDARD} {DEBUG} {INTERPRETER_SRC} {FLAGS_MAIN} -o lib{INTERPRETER_BIN}.so\n')
                f.write(f'echo \'  {CC_WIN32} -std={CXX_STANDARD} {DEBUG} {INTERPRETER_SRC} {FLAGS_INTERPRETER_WIN32} -o {INTERPRETER_BIN}.exe\'\n')
                f.write(f'{CC_WIN32} -std={CXX_STANDARD} {DEBUG} {INTERPRETER_SRC} {FLAGS_INTERPRETER_WIN32} -o {INTERPRETER_BIN}.exe\n')
               
                for index, module in enumerate(MODULES):
                    f.write(f'echo \'Building library "{module}" ... ({index + 2} / {total}) -> yan-{module}.dll\'\n')
                    f.write(f'echo \'  {CC_WIN32} -std={CXX_STANDARD} {DEBUG} yan-lib{module}.cpp {FLAGS_LIBS_WIN32} -o {DYLIB_PATH}\\yan-{module}.dll\'\n')
                    f.write(f'{CC_WIN32} -std={CXX_STANDARD} {DEBUG} yan-lib{module}.cpp {FLAGS_LIBS_WIN32} -o  {DYLIB_PATH}\\yan-{module}.dll\n')
            os.system('chmod +x build-win32.sh')
        else:
            sys.stderr.write(f'Error: Invilid platform: \'{sys.argv[2][1:]}\'\n')
    elif len(sys.argv) == 2 and sys.argv[1] == 'clean':
        os.system('rm yan yan.exe')
        for module in MODULES:
            os.system(f'rm yan-{module}.so yan-{module}.dll')
    elif len(sys.argv) == 2:
        if sys.argv[1] == 'linux':
            os.system('./build.sh')
        elif sys.argv[1] == 'win32':
            os.system('./build-win32.sh')
        else:
            sys.stderr.write(f'Error: Invilid platform: \'{sys.argv[1]}\'\n')
    else:
        sys.stderr.write('Error: Invilid usage\n')
