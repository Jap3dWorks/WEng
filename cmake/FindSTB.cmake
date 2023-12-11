# FindSTB.cmake

IF(UNIX)
    # find_path(STB_INCLUDE_DIRS NAMES stb.h PATHS /usr/include/stb)

    # include(FindPackageHandleStandardArgs)
    # find_package_handle_standard_args(STB DEFAULT_MSG STB_INCLUDE_DIRS)

    # mark_as_advanced(STB_INCLUDE_DIRS)

    set(STB_INCLUDE_DIRS "/usr/include")
ENDIF()