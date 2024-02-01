IF(UNIX)

    # find_package(tinyobjloader REQUIRED)
    find_path(tinyobjloader_INCLUDE_DIRS tiny_obj_loader.h)

    if(tinyobjloader_INCLUDE_DIRS)

        message(STATUS "tinyobjloader found")
        set(tinyobjloader_FOUND TRUE)
    
    else()

        set(tinyobjloader_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/../tinyobjloader)
        # check if tiniobjloader directory exists
        IF(NOT EXISTS ${tinyobjloader_INCLUDE_DIRS})
            message(FATAL_ERROR "tinyobjloader directory not found at ${tinyobjloader_INCLUDE_DIRS}")
        ELSE()
            message(STATUS "tinyobjloader directory found at ${tinyobjloader_INCLUDE_DIRS}")
            set(tinyobjloader_FOUND TRUE)
        ENDIF()

    ENDif()

ENDIF()
