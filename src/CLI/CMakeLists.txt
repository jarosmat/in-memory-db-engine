cmake_minimum_required(VERSION 4.0)
project(project)

set(CMAKE_CXX_STANDARD 23)

set(DBTYPE_DIR "${PROJECT_SOURCE_DIR}")

add_library(DBType "${DBTYPE_DIR}/DBType.cpp")
target_include_directories(DBType PUBLIC "${SWISS_TABLE_DIR}" "${HASH_FUNCTION_DIR}" "${DBENTRY_DIR}" "${TIMEPOINT_DIR}")
target_link_libraries(DBType INTERFACE DBEntry SwissTable HashFunc Timepoint)
