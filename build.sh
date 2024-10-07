#!/bin/bash
echo 'Building interpreter ... (1 / 8) -> yan'
echo '  g++ -std=c++20 -g yan-main.cpp  -o yan'
g++ -std=c++20 -g yan-main.cpp  -o yan
echo 'Building library "rand" ... (2 / 8) -> yan-rand.so'
echo '  g++ -std=c++20 -g yan-librand.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-rand.so'
g++ -std=c++20 -g yan-librand.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-rand.so
echo 'Building library "fs" ... (3 / 8) -> yan-fs.so'
echo '  g++ -std=c++20 -g yan-libfs.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-fs.so'
g++ -std=c++20 -g yan-libfs.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-fs.so
echo 'Building library "string" ... (4 / 8) -> yan-string.so'
echo '  g++ -std=c++20 -g yan-libstring.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-string.so'
g++ -std=c++20 -g yan-libstring.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-string.so
echo 'Building library "inspect" ... (5 / 8) -> yan-inspect.so'
echo '  g++ -std=c++20 -g yan-libinspect.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-inspect.so'
g++ -std=c++20 -g yan-libinspect.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-inspect.so
echo 'Building library "os" ... (6 / 8) -> yan-os.so'
echo '  g++ -std=c++20 -g yan-libos.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-os.so'
g++ -std=c++20 -g yan-libos.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-os.so
echo 'Building library "time" ... (7 / 8) -> yan-time.so'
echo '  g++ -std=c++20 -g yan-libtime.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-time.so'
g++ -std=c++20 -g yan-libtime.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-time.so
echo 'Building library "sdl2" ... (8 / 8) -> yan-sdl2.so'
echo '  g++ -std=c++20 -g yan-libsdl2.cpp -shared -lSDL2 -fPIC -rdynamic -ldl -o lib//yan-sdl2.so'
g++ -std=c++20 -g yan-libsdl2.cpp -shared -lSDL2 -fPIC -rdynamic -ldl -o lib//yan-sdl2.so
