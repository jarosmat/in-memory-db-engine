cmake_minimum_required(VERSION 4.0)
project(project)

set(CMAKE_CXX_STANDARD 23)
set(UTILS_DIR "${PROJECT_SOURCE_DIR}")

add_library(GlobalUtils Constants.cpp)
target_include_directories(GlobalUtils PUBLIC "${UTILS_DIR}")
