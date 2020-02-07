macro(create_lib name source_files)
    add_library(${name} ${source_files})
    target_include_directories(${name} PUBLIC ${CMAKE_CURRENT_LIST_DIR})

    # standard warnings
    target_compile_options(${name} PRIVATE
            -Wall -Wextra # reasonable and standard
            -Wshadow # warn the user if a variable declaration shadows one from a parent context
            -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This
            # helps catch hard to track down memory errors
            -Wcast-align # warn for potential performance problem casts
            -Wunused # warn on anything being unused
            -Woverloaded-virtual # warn if you overload (not override) a virtual function
            -Wpedantic # (all versions of GCC, Clang >= 3.2) warn if non-standard C++ is used
            -Wconversion # warn on type conversions that may lose data
            -Wdouble-promotion # (GCC >= 4.6, Clang >= 3.8) warn if float is implicit promoted to double
            -Wformat=2 # warn on security issues around functions that format output (ie printf)
            # -Wlifetime (only special branch of Clang currently) shows object lifetime issues
            )

    # gcc specific warnings
    if (${CMAKE_COMPILER_IS_GNUCXX})
        target_compile_options(${name}
                PRIVATE
                -Wmisleading-indentation # (only in GCC >= 6.0) warn if indentation implies blocks where blocks do not exist
                -Wduplicated-cond # (only in GCC >= 6.0) warn if if / else chain has duplicated conditions
                -Wduplicated-branches # (only in GCC >= 7.0) warn if if / else branches have duplicated code
                -Wlogical-op # (only in GCC) warn about logical operations being used where bitwise were probably wanted
                -Wnull-dereference # (only in GCC >= 6.0) warn if a null dereference is detected
                -Wuseless-cast # (only in GCC >= 4.8) warn if you perform a cast to the same type
                )
    endif ()
endmacro()


set(xenon_src_files
        gui/window.cpp
        gui/window.hpp
        misc/hack_jthread.cpp
        misc/hack_jthread.hpp
        network/network.cpp
        network/network.hpp
        )

create_lib(xenon_core "${xenon_src_files}")
target_link_libraries(xenon_core
        PUBLIC
        PkgConfig::gtkmm
        skyr-url
        httplib
        pthread
        )

add_executable(xenon
        main.cpp)
target_link_libraries(xenon PRIVATE xenon_core)
