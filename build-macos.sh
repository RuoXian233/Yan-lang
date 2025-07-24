#!/bin/bash
echo 'Building interpreter ... (1 / 9) -> yan'
echo '  g++ -std=c++20 -g yan-main.cpp  -o yan'
g++ -std=c++20 -g yan-main.cpp  -o yan
echo 'Building library "rand" ... (2 / 9) -> yan-rand.dylib'
echo '  g++ -std=c++20 -g yan-librand.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-rand.dylib'
g++ -std=c++20 -g yan-librand.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-rand.dylib
echo 'Building library "fs" ... (3 / 9) -> yan-fs.dylib'
echo '  g++ -std=c++20 -g yan-libfs.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-fs.dylib'
g++ -std=c++20 -g yan-libfs.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-fs.dylib
echo 'Building library "string" ... (4 / 9) -> yan-string.dylib'
echo '  g++ -std=c++20 -g yan-libstring.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-string.dylib'
g++ -std=c++20 -g yan-libstring.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-string.dylib
echo 'Building library "inspect" ... (5 / 9) -> yan-inspect.dylib'
echo '  g++ -std=c++20 -g yan-libinspect.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-inspect.dylib'
g++ -std=c++20 -g yan-libinspect.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-inspect.dylib
echo 'Building library "os" ... (6 / 9) -> yan-os.dylib'
echo '  g++ -std=c++20 -g yan-libos.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-os.dylib'
g++ -std=c++20 -g yan-libos.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-os.dylib
echo 'Building library "time" ... (7 / 9) -> yan-time.dylib'
echo '  g++ -std=c++20 -g yan-libtime.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-time.dylib'
g++ -std=c++20 -g yan-libtime.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-time.dylib
echo 'Building library "sdl2" ... (8 / 9) -> yan-sdl2.dylib'
echo '  g++ -std=c++20 -g yan-libsdl2.cpp -shared -fPIC -rdynamic -lSDL2 -ldl -o lib//yan-sdl2.dylib'
# g++ -std=c++20 -g yan-libsdl2.cpp -shared -fPIC -rdynamic -lSDL2 -ldl -o lib//yan-sdl2.dylib
echo 'WARNING: Skipped on platform darwin (aarch64)' 
echo 'Building library "json" ... (9 / 9) -> yan-json.dylib'
echo '  g++ -std=c++20 -g yan-libjson.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-json.dylib'
g++ -std=c++20 -g yan-libjson.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-json.dylib
