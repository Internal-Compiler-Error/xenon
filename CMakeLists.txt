cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(xenon VERSION 0.1 LANGUAGES CXX)
enable_testing()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS FALSE)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_VERBOSE_MAKEFILE FALSE)


add_subdirectory(extern)
add_subdirectory(src)


add_subdirectory(test)


