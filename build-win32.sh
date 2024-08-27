#!/bin/bash
echo 'Building interpreter ... (1 / 7) -> yan'
echo '  x86_64-w64-mingw32-g++ -std=c++20 -g yan-main.cpp  -o yan.exe'
x86_64-w64-mingw32-g++ -std=c++20 -g yan-main.cpp  -o yan.exe
echo 'Building library "rand" ... (2 / 7) -> yan-rand.dll'
echo '  x86_64-w64-mingw32-g++ -std=c++20 -g yan-librand.cpp -shared -fPIC -o lib/\yan-rand.dll'
x86_64-w64-mingw32-g++ -std=c++20 -g yan-librand.cpp -shared -fPIC -o  lib/\yan-rand.dll
echo 'Building library "fs" ... (3 / 7) -> yan-fs.dll'
echo '  x86_64-w64-mingw32-g++ -std=c++20 -g yan-libfs.cpp -shared -fPIC -o lib/\yan-fs.dll'
x86_64-w64-mingw32-g++ -std=c++20 -g yan-libfs.cpp -shared -fPIC -o  lib/\yan-fs.dll
echo 'Building library "string" ... (4 / 7) -> yan-string.dll'
echo '  x86_64-w64-mingw32-g++ -std=c++20 -g yan-libstring.cpp -shared -fPIC -o lib/\yan-string.dll'
x86_64-w64-mingw32-g++ -std=c++20 -g yan-libstring.cpp -shared -fPIC -o  lib/\yan-string.dll
echo 'Building library "inspect" ... (5 / 7) -> yan-inspect.dll'
echo '  x86_64-w64-mingw32-g++ -std=c++20 -g yan-libinspect.cpp -shared -fPIC -o lib/\yan-inspect.dll'
x86_64-w64-mingw32-g++ -std=c++20 -g yan-libinspect.cpp -shared -fPIC -o  lib/\yan-inspect.dll
echo 'Building library "os" ... (6 / 7) -> yan-os.dll'
echo '  x86_64-w64-mingw32-g++ -std=c++20 -g yan-libos.cpp -shared -fPIC -o lib/\yan-os.dll'
x86_64-w64-mingw32-g++ -std=c++20 -g yan-libos.cpp -shared -fPIC -o  lib/\yan-os.dll
echo 'Building library "time" ... (7 / 7) -> yan-time.dll'
echo '  x86_64-w64-mingw32-g++ -std=c++20 -g yan-libtime.cpp -shared -fPIC -o lib/\yan-time.dll'
x86_64-w64-mingw32-g++ -std=c++20 -g yan-libtime.cpp -shared -fPIC -o  lib/\yan-time.dll
