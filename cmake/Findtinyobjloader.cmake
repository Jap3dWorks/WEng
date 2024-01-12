IF(UNIX)

    set(TINYOBJLOADER_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/../tinyobjloader)
    # check if tiniobjloader directory exists
    IF(NOT EXISTS ${TINYOBJLOADER_INCLUDE_DIRS})
        message(FATAL_ERROR "tiniobjloader directory not found at ${TINIOBJLOADER_INCLUDE_DIRS}")
    ENDIF()

ENDIF()
