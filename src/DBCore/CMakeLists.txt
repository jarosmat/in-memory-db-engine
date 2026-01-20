cmake_minimum_required(VERSION 4.0)
project(project)

set(CMAKE_CXX_STANDARD 23)

add_library(Entries DBEntry/String.hpp DBEntry/String.cpp DBEntry/PrefixAut.hpp DBEntry/PrefixAut.cpp)

add_executable(StringTest DBEntry/StringTest.cpp)
target_link_libraries(StringTest Entries Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(StringTest)