# GTKMM
find_package(PkgConfig)
pkg_check_modules(gtkmm REQUIRED QUIET IMPORTED_TARGET gtkmm-3.0)
if (gtkmm_FOUND)
    set_target_properties(PkgConfig::gtkmm PROPERTIES IMPORTED_GLOBAL TRUE)
else ()
    message(FATAL_ERROR "GTKMM IS REQUIRED")
endif ()

# below is all the git submodules live
add_subdirectory(url)
add_subdirectory(httplib-cheat)
