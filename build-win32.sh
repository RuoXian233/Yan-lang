#!/bin/bash
echo 'Building interpreter ... (1 / 3) -> yan'
echo '  x86_64-w64-mingw32-g++ -std=c++20 -g yan-main.cpp  -o yan.exe'
x86_64-w64-mingw32-g++ -std=c++20 -g yan-main.cpp  -o yan.exe
echo 'Building library "rand" ... (2 / 3) -> yan-rand.dll'
echo '  x86_64-w64-mingw32-g++ -std=c++20 -g yan-librand.cpp -shared -fPIC -o lib/\yan-rand.dll'
x86_64-w64-mingw32-g++ -std=c++20 -g yan-librand.cpp -shared -fPIC -o  lib/\yan-rand.dll
echo 'Building library "fs" ... (3 / 3) -> yan-fs.dll'
echo '  x86_64-w64-mingw32-g++ -std=c++20 -g yan-libfs.cpp -shared -fPIC -o lib/\yan-fs.dll'
x86_64-w64-mingw32-g++ -std=c++20 -g yan-libfs.cpp -shared -fPIC -o  lib/\yan-fs.dll
