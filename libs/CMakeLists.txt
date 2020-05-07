cmake_minimum_required (VERSION 3.8)

project(libs)

file (GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
                           "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
                           "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

include_directories (${Headers})

add_library(libs STATIC ${SOURCES})

add_custom_command(TARGET libs POST_BUILD COMMAND ${INC_BUILD})
