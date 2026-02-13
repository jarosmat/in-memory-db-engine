cmake_minimum_required(VERSION 4.0)
project(project)

set(CMAKE_CXX_STANDARD 23)

add_library(Entries DBEntry/String.hpp DBEntry/String.cpp DBEntry/PrefixAut.hpp DBEntry/PrefixAut.cpp)
add_library(SwissTable SwissTable/VectorClass.hpp SwissTable/Vector.hpp SwissTable/Vector.tpp SwissTable/Group.cpp
        SwissTable/Group.hpp SwissTable/Entry.hpp SwissTable/EntryClass.hpp SwissTable/Entry.tpp SwissTable/SwissTable.hpp SwissTable/SwissTableClass.hpp
        SwissTable/SwissTable.tpp)

add_executable(StringTest DBEntry/StringTest.cpp)
target_link_libraries(StringTest Entries Catch2::Catch2WithMain)

add_executable(VectorTest SwissTable/VectorTest.cpp)
target_link_libraries(VectorTest Entries Catch2::Catch2WithMain)

add_executable(GroupTest SwissTable/GroupTest.cpp)
target_link_libraries(GroupTest Entries SwissTable Catch2::Catch2WithMain)

add_executable(SwissTableTest SwissTable/SwissTableTest.cpp SwissTable/SwissTable.hpp)
target_link_libraries(SwissTableTest Entries SwissTable Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(StringTest)