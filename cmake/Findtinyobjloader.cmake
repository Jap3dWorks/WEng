set(tinyobjloader_FOUND FALSE)

IF(UNIX)

    find_path(
        tinyobjloader_INCLUDE_DIRS 
        NAMES "tiny_obj_loader.h"
    )

    if(
        NOT "${tinyobjloader_INCLUDE_DIRS}" STREQUAL "tinyobjloader_INCLUDE_DIRS-NOTFOUND" AND
        EXISTS "${tinyobjloader_INCLUDE_DIRS}/tiny_obj_loader.h"
    )
        message(STATUS "tiny_obj_loader.h found at: ${tinyobjloader_INCLUDE_DIRS}")
        set(tinyobjloader_FOUND TRUE)
    
    else()

        set(tinyobjloader_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/../tinyobjloader)

        # check if tiniobjloader directory exists
        IF(NOT EXISTS ${tinyobjloader_INCLUDE_DIRS})
            message(FATAL_ERROR "tiny_obj_loader.h directory not found at ${tinyobjloader_INCLUDE_DIRS}")
        ELSE()
            message(STATUS "tiny_obj_loader directory found at ${tinyobjloader_INCLUDE_DIRS}")
            set(tinyobjloader_FOUND TRUE)
        ENDIF()

    ENDif()

ELSE()

    message(FATAL_ERROR "Not Valid Platform.")

ENDIF()
