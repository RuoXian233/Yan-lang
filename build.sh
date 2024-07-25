#!/bin/bash
echo 'Building interpreter ... (1 / 3) -> yan'
echo '  g++ -std=c++20 -g yan-main.cpp  -o yan'
g++ -std=c++20 -g yan-main.cpp  -o yan
echo 'Building library "rand" ... (2 / 3) -> yan-rand.so'
echo '  g++ -std=c++20 -g yan-librand.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-rand.so'
g++ -std=c++20 -g yan-librand.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-rand.so
echo 'Building library "fs" ... (3 / 3) -> yan-fs.so'
echo '  g++ -std=c++20 -g yan-libfs.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-fs.so'
g++ -std=c++20 -g yan-libfs.cpp -shared -fPIC -rdynamic -ldl -o lib//yan-fs.so
