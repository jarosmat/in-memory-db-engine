@echo off
cmake -S . -B build_test -DCMAKE_BUILD_TYPE=Debug
cmake --build build_test --config Debug
pause