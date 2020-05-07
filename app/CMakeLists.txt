cmake_minimum_required (VERSION 3.8)

project(slovo C ASM)

file (GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
                           "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
                           "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable (slovo.elf ${SOURCES})

include_directories (${Headers})

target_link_libraries(slovo.elf -Wl,--whole-archive libs -Wl,--no-whole-archive)
