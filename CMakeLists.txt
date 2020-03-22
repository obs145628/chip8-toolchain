cmake_minimum_required(VERSION 3.0)
project(chip8-toolchain)

include_directories("./include/")

set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -Werror -pedantic -O0 -g3")

set(CMAKE_CXX_COMPILER)
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Werror -pedantic -O0 -g3")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src/oc8_is)
