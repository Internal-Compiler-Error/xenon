cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(xenon VERSION 0.1 LANGUAGES CXX)

option(XENON_ENABLE_TEST "Build with tests" ON)
option(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS FALSE)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(extern)
add_subdirectory(src)

if (XENON_ENABLE_TEST)
    enable_testing()
    add_subdirectory(test)
endif ()
