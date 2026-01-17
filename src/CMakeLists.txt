cmake_minimum_required(VERSION 4.0)
project(project)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory("${PROJECT_SOURCE_DIR}/CLI")
add_subdirectory("${PROJECT_SOURCE_DIR}/DBCore")