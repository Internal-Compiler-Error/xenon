add_library(httplib INTERFACE IMPORTED GLOBAL)
target_include_directories(httplib SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})