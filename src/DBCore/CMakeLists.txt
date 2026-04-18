cmake_minimum_required(VERSION 4.0)
project(project)

set(CMAKE_CXX_STANDARD 23)
set(STRING_DIR "${PROJECT_SOURCE_DIR}/DBEntry/String")
set(ENTRY_DIR "${PROJECT_SOURCE_DIR}/SwissTable/Entry")
set(GROUP_DIR "${PROJECT_SOURCE_DIR}/SwissTable/Group")
set(VECTOR_DIR "${PROJECT_SOURCE_DIR}/SwissTable/Vector")
set(SWISS_TABLE_DIR "${PROJECT_SOURCE_DIR}/SwissTable")
set(HASH_FUNCTION_DIR "${PROJECT_SOURCE_DIR}/HashFunction")

add_library(Entries "${STRING_DIR}/String.cpp"  "${STRING_DIR}/PrefixAut.cpp")
target_include_directories(Entries PUBLIC "${STRING_DIR}")


add_library(SwissTable SwissTable/Group/Group.cpp)
target_include_directories(SwissTable PUBLIC "${VECTOR_DIR}" "${GROUP_DIR}" "${ENTRY_DIR}" "${SWISS_TABLE_DIR}")


add_library(HashFunc "${HASH_FUNCTION_DIR}/SipHash.cpp")
target_include_directories(HashFunc PUBLIC "${HASH_FUNCTION_DIR}")


