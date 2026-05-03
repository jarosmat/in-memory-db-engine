#!/bin/sh

mkdir build_test
cd build_test || exit
ls

cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC" ..
cmake --build .