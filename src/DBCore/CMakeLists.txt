cmake_minimum_required(VERSION 4.0)
project(project)

set(CMAKE_CXX_STANDARD 23)

add_library(Entry DBEntry/String.hpp DBEntry/String.cpp)