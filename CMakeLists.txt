cmake_minimum_required (VERSION 3.8)

include ("${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain.cmake")
include ("${CMAKE_CURRENT_SOURCE_DIR}/cmake/functions.cmake")

project (slovo)

header_directories (ALL_LIBRARY_HEADER_DIRECTORIES)
set (Headers ${ALL_LIBRARY_HEADER_DIRECTORIES})

add_subdirectory (libs)
add_subdirectory (app)