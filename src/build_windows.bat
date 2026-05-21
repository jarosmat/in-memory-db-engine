@echo off
cmake -S . -B build_test -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
pause