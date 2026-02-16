cmake_minimum_required(VERSION 4.0)
project(project)

set(CMAKE_CXX_STANDARD 23)
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/IncludeDirectory")
set(TEST_DIRECTORY "${PROJECT_SOURCE_DIR}/Tests")


include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

add_subdirectory("${PROJECT_SOURCE_DIR}/CLI")
add_subdirectory("${PROJECT_SOURCE_DIR}/DBCore")


add_subdirectory("${TEST_DIRECTORY}")

add_executable(main main.cpp)
target_link_libraries(main Entries SwissTable)