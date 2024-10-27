cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE cmake/toolchain.cmake)
project(AVR LANGUAGES C CXX ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_VERBOSE_MAKEFILE n)

add_subdirectory(src)