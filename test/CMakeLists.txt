enable_testing()

#add_executable(gui_test gui_test.cpp)
add_executable(network_test network_test.cpp)
add_executable(hack_jthread_test hack_jthread_test.cpp)

find_package(Catch2 CONFIG REQUIRED)
#target_link_libraries(gui_test PRIVATE Catch2::Catch2 gui)
target_link_libraries(hack_jthread_test PRIVATE Catch2::Catch2 hack_jthread)


find_package(GTest CONFIG REQUIRED)

target_link_libraries(network_test PRIVATE Catch2::Catch2 network)
add_test(NAME all_tests_in_gui_test
        COMMAND gui_test
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_test(all_tests_in_jthread_tests hack_jthread_test)

add_test(all_tests_in_network_test
        COMMAND network_test
        WORKING_DIRECTORY test_dump)