add_library(network network.hpp network.cpp)
target_link_libraries(network PUBLIC skyr-url gui)
target_link_libraries(network PUBLIC httplib hack_jthread)
target_include_directories(network INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(network PUBLIC
        -Wall
        -Wextra
        -Wpedantic
        )