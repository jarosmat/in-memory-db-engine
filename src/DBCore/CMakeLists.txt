cmake_minimum_required(VERSION 4.0)
project(project)

set(CMAKE_CXX_STANDARD 23)
set(STRING_DIR "${PROJECT_SOURCE_DIR}/DBEntry/String")
set(ENTRY_DIR "${PROJECT_SOURCE_DIR}/SwissTable/Entry")
set(GROUP_DIR "${PROJECT_SOURCE_DIR}/SwissTable/Group")
set(VECTOR_DIR "${PROJECT_SOURCE_DIR}/SwissTable/Vector")
set(SWISS_TABLE_DIR "${PROJECT_SOURCE_DIR}/SwissTable")
set(HASH_FUNCTION_DIR "${PROJECT_SOURCE_DIR}/HashFunction")
set(OBJECT_DIR "${PROJECT_SOURCE_DIR}/DBEntry/Object")
set(TIMEPOINT_DIR "${PROJECT_SOURCE_DIR}/DBEntry/Timepoint")

add_library(Timepoint "${TIMEPOINT_DIR}/Timepoint.cpp")
target_include_directories(Timepoint PUBLIC "${TIMEPOINT_DIR}")

add_library(Entries "${STRING_DIR}/String.cpp" "${STRING_DIR}/PrefixAut.cpp" "${OBJECT_DIR}/Object.cpp")
target_include_directories(Entries PUBLIC "${STRING_DIR}" "${PROJECT_SOURCE_DIR}/DBEntry" "${OBJECT_DIR}" "${VECTOR_DIR}")

add_library(SwissTable "${GROUP_DIR}/Group.cpp")
target_include_directories(SwissTable PUBLIC "${VECTOR_DIR}" "${GROUP_DIR}" "${ENTRY_DIR}" "${SWISS_TABLE_DIR}")

add_library(HashFunc "${HASH_FUNCTION_DIR}/SipHash.cpp")
target_include_directories(HashFunc PUBLIC "${HASH_FUNCTION_DIR}")

add_library(DBEntry "${PROJECT_SOURCE_DIR}/DBEntry/DBEntry.cpp")
target_link_libraries(DBEntry Entries SwissTable)

add_library(DBEntry INTERFACE "${PROJECT_SOURCE_DIR}/DBEntry/DBEntry.hpp")
target_include_directories(DBEntry INTERFACE "${TIMEPOINT_DIR}" "${SWISS_TABLE_DIR}")
target_link_libraries(DBEntry INTERFACE Entries SwissTable Timepoint)
