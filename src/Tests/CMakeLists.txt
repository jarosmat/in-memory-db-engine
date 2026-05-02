cmake_minimum_required(VERSION 4.0)
project(project)

set(CMAKE_CXX_STANDARD 23)

include(CTest)


add_executable(StringTest StringTest.cpp)
target_link_libraries(StringTest Entries Catch2::Catch2WithMain)
add_test(NAME StringTest COMMAND StringTest)

add_executable(VectorTest VectorTest.cpp)
target_link_libraries(VectorTest Entries SwissTable Catch2::Catch2WithMain)
add_test(NAME VectorTest COMMAND VectorTest)

add_executable(GroupTest GroupTest.cpp)
target_link_libraries(GroupTest Entries SwissTable Catch2::Catch2WithMain)
add_test(NAME GroupTest COMMAND GroupTest)

add_executable(SwissTableTest SwissTableTest.cpp)
target_link_libraries(SwissTableTest Entries SwissTable Catch2::Catch2WithMain)
add_test(NAME SwissTableTest COMMAND SwissTableTest)

add_executable(ObjectTest ObjectTest.cpp)
target_link_libraries(ObjectTest Entries Catch2::Catch2WithMain)
add_test(NAME ObjectTest COMMAND ObjectTest)

add_executable(TimepointTest TimepointTest.cpp)
target_link_libraries(TimepointTest Timepoint Catch2::Catch2WithMain)
add_test(NAME TimepointTest COMMAND TimepointTest)

add_executable(DBEntryTest DBEntryTest.cpp)
target_link_libraries(DBEntryTest DBEntry Catch2::Catch2WithMain)
add_test(NAME DBEntryTest COMMAND DBEntryTest)

add_executable(ParserTest ParserTest.cpp)
target_link_libraries(ParserTest Parser Catch2::Catch2WithMain)
add_test(NAME ParserTest COMMAND ParserTest)


include(Catch)
catch_discover_tests(StringTest VectorTest GroupTest SwissTableTest ObjectTest TimepointTest DBEntryTest ParserTest)
