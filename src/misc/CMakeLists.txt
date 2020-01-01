add_library(hack_jthread hack_jthread.cpp hack_jthread.hpp)
target_include_directories(hack_jthread INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hack_jthread INTERFACE pthread)
target_compile_options(hack_jthread PRIVATE
        -Wall
        -Wextra
        -Wpedantic)