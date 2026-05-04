cmake_minimum_required(VERSION 4.0)
project(project)

set(CMAKE_CXX_STANDARD 23)

set(DBTYPE_DIR "${PROJECT_SOURCE_DIR}")
set(PARSER_DIR "${PROJECT_SOURCE_DIR}")
set(TTL_QUEUE_ENTRY_DIR "${PROJECT_SOURCE_DIR}")
set(ERROR_DIR "${PROJECT_SOURCE_DIR}")
set(UTILS_DIR "${PROJECT_SOURCE_DIR}")


add_library(TTL_QueueEntry QueueEntry.cpp)
target_include_directories(TTL_QueueEntry PUBLIC "${TTL_QUEUE_ENTRY_DIR}")
target_link_libraries(TTL_QueueEntry PUBLIC Timepoint)

add_library(Utils Utils.cpp)
target_include_directories(Utils PUBLIC "${UTILS_DIR}")

add_library(Error Error.cpp)
target_include_directories(Error PUBLIC "${ERROR_DOR}")

add_library(DBType "${DBTYPE_DIR}/DBType.cpp" "${DBTYPE_DIR}/DBType.hpp")
target_include_directories(DBType PUBLIC "${SWISS_TABLE_DIR}" "${HASH_FUNCTION_DIR}" "${DBENTRY_DIR}" "${TIMEPOINT_DIR}")
target_link_libraries(DBType PUBLIC DBEntry SwissTable HashFunc Timepoint)

add_library(Nodes Nodes.cpp)
target_include_directories(Nodes PUBLIC "${SWISS_TABLE_DIR}" "${STRING_DIR}" "${HASH_FUNCTION_DIR}")
target_link_libraries(Nodes PUBLIC Entries DBEntry HashFunc)

add_library(Parser "${PARSER_DIR}/OperationType.cpp" "${PARSER_DIR}/OperationType.hpp")
target_include_directories(Parser PUBLIC "${PARSER_DIR}" "${VALUE_TYPES_DIR}" "${VECTOR_DIR}" "${SWISS_TABLE_DIR}" "${DBENTRY_DIR}")
target_link_libraries(Parser PUBLIC Entries SwissTable DBEntry HashFunc DBType TTL_QueueEntry Error Utils)

