add_library(gui window.hpp window.cpp)

target_link_libraries(gui PUBLIC PkgConfig::gtkmm)
target_link_libraries(gui PUBLIC network)
target_link_libraries(gui PRIVATE skyr-url)
target_include_directories(gui PUBLIC ${CMAKE_CURRENT_LIST_DIR})

#add_dependencies(window copy_glade_files)
