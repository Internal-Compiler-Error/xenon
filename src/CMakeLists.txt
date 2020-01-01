add_subdirectory(gui)
add_subdirectory(network)
add_subdirectory(resources)
add_subdirectory(misc)

add_executable(xenon
        main.cpp)


target_link_libraries(xenon PUBLIC gui)
