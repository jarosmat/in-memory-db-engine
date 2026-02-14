cmake_minimum_required(VERSION 4.0)
project(project)

set(CMAKE_CXX_STANDARD 23)
set(STRING_DIR "${PROJECT_SOURCE_DIR}/DBEntry/String")
set(ENTRY_DIR "${PROJECT_SOURCE_DIR}/SwissTable/Entry")
set(GROUP_DIR "${PROJECT_SOURCE_DIR}/SwissTable/Group")
set(VECTOR_DIR "${PROJECT_SOURCE_DIR}/SwissTable/Vector")
set(SWISS_TABLE_DIR "${PROJECT_SOURCE_DIR}/SwissTable")

add_library(Entries "${STRING_DIR}/String.cpp"  "${STRING_DIR}/PrefixAut.cpp")
target_include_directories(Entries PUBLIC "${STRING_DIR}")


add_library(SwissTable SwissTable/Group/Group.cpp)
target_include_directories(SwissTable PUBLIC "${VECTOR_DIR}" "${GROUP_DIR}" "${ENTRY_DIR}" "${SWISS_TABLE_DIR}")

add_executable(StringTest DBEntry/StringTest.cpp)
target_link_libraries(StringTest Entries Catch2::Catch2WithMain)

add_executable(VectorTest SwissTable/VectorTest.cpp)
target_link_libraries(VectorTest Entries Catch2::Catch2WithMain)
target_include_directories(VectorTest PUBLIC "${VECTOR_DIR}")

add_executable(GroupTest SwissTable/GroupTest.cpp)
target_link_libraries(GroupTest Entries SwissTable Catch2::Catch2WithMain)

add_executable(SwissTableTest SwissTable/SwissTableTest.cpp SwissTable/SwissTable.hpp)
target_link_libraries(SwissTableTest Entries SwissTable Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(StringTest)