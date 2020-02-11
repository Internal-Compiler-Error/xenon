add_library(Catch2_main OBJECT catch2_main.cpp)
target_link_libraries(Catch2_main PUBLIC Catch2::Catch2)

macro(create_test name source_files working_directory)
    set(test_name "${name}_test")
    add_executable(${test_name} ${source_files})
    target_link_libraries(${test_name}
            PRIVATE
            Catch2_main xenon_core Catch2::Catch2
            )
    add_test("all_tests_in_${test_name}"
            COMMAND "${test_name}"
            WORKING_DIRECTORY "${working_directory}"
            )
endmacro()

create_test(network network_test.cpp test_dump)
create_test(hack_jthread hack_jthread_test.cpp test_dump)
