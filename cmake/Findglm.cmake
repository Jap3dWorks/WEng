
include(FindPackageHandleStandardArgs)


if (WIN32)
    find_path(glm_DIR
        NAMES
            glmConfig.cmake glm-Config.cmake
        PATHS
            $ENV{glm_DIR}
            C:/msys64/mingw64/lib/cmake/glm
        DOC "The glm Config File"
        REQUIRED
    )
elseif(UNIX)
    find_path(glm_DIR
        NAMES
            glmConfig.cmake glm-Config.cmake
        PATHS
            $ENV{glm_DIR}
            /usr/lib/cmake/glm
        DOC "The glm Config File"
        REQUIRED
)
endif()

find_package(glm CONFIG REQUIRED)
