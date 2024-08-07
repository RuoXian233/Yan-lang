#!/bin/bash
echo 'Building interpreter ... (1 / 4) -> yan'
echo '  x86_64-w64-mingw32-g++ -std=c++20 -g yan-main.cpp  -o yan.exe'
x86_64-w64-mingw32-g++ -std=c++20 -g yan-main.cpp  -o yan.exe
echo 'Building library "rand" ... (2 / 4) -> yan-rand.dll'
echo '  x86_64-w64-mingw32-g++ -std=c++20 -g yan-librand.cpp -shared -fPIC -o lib/\yan-rand.dll'
x86_64-w64-mingw32-g++ -std=c++20 -g yan-librand.cpp -shared -fPIC -o  lib/\yan-rand.dll
echo 'Building library "fs" ... (3 / 4) -> yan-fs.dll'
echo '  x86_64-w64-mingw32-g++ -std=c++20 -g yan-libfs.cpp -shared -fPIC -o lib/\yan-fs.dll'
x86_64-w64-mingw32-g++ -std=c++20 -g yan-libfs.cpp -shared -fPIC -o  lib/\yan-fs.dll
echo 'Building library "string" ... (4 / 4) -> yan-string.dll'
echo '  x86_64-w64-mingw32-g++ -std=c++20 -g yan-libstring.cpp -shared -fPIC -o lib/\yan-string.dll'
x86_64-w64-mingw32-g++ -std=c++20 -g yan-libstring.cpp -shared -fPIC -o  lib/\yan-string.dll
