@echo off
mkdir build_win64 
cd build_win64
cmake -G "Visual Studio 16 2019" ..
cd ..