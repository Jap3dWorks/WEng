
include(FindPackageHandleStandardArgs)


if (WIN32)
    find_path(SDL2_DIR
        NAMES
            SDL2Config.cmake SDL2-Config.cmake
        PATHS
            $ENV{SDL2_DIR}
            C:/msys64/mingw64/lib/cmake/SDL2
        DOC "The SDL2 Config File"
        REQUIRED
    )
elseif(UNIX)
    find_path(SDL2_DIR
        NAMES
            SDL2Config.cmake SDL2-Config.cmake
        PATHS
            $ENV{SDL2_DIR}
            /usr/lib/cmake/SDL2
        DOC "The SDL2 Config File"
        REQUIRED
)
endif()


find_package(SDL2 CONFIG REQUIRED)



# find_package(SDL2 CONFIG )


# if (WIN32)
#     message(STATUS "SDL2 From Windows")
#     find_path( SDL2_INCLUDE_DIR
#         NAMES
#             SDL2/SDL.h
#         PATHS
#             ${SDL2_LOCATION}/include
#             $ENV{SDL2_LOCATION}/include
#             $ENV{PROGRAMFILES}/SDL2/include
#             C:/msys64/mingw64/include
#         DOC "The directory where SDL2/sdl.h resides")
#     if(ARCH STREQUAL "x86")
#         find_library( SDL2_LIBRARY
#             NAMES
#                 SDL2
#             PATHS
#                 ${SDL2_LOCATION}/lib
#                 ${SDL2_LOCATION}/lib/x86
#                 ${SDL2_LOCATION}/lib/win32
#                 ${SDL2_LOCATION}/lib/Release/win32
#                 ${SDL2_LOCATION}/lib/Release MX/win32
#                 $ENV{SDL2_LOCATION}/lib
#                 $ENV{SDL2_LOCATION}/lib/Release/win32
#                 $ENV{SDL2_LOCATION}/lib/Release MX/win32
#                 $ENV{SDL2_LOCATION}/lib/x86
#                 $ENV{SDL2_LOCATION}/lib/win32
#                 $ENV{PROGRAMFILES}/SDL2/lib
#                 $ENV{PROGRAMFILES}/SDL2/lib/x86
#                 $ENV{PROGRAMFILES}/SDL2/lib/win32
#                 ${SDL2_LOCATION}
#                 $ENV{SDL2_LOCATION}
#                 c:/msys64/mingw32/lib
#             DOC "The SDL2 library")
#     else()
#         find_library( SDL2_LIBRARY
#             NAMES
#                 SDL2 libSDL2
#             PATHS
#                 ${SDL2_LOCATION}/lib/x64
#                 ${SDL2_LOCATION}/lib/Release/x64
#                 ${SDL2_LOCATION}/lib/Release MX/x64
#                 $ENV{SDL2_LOCATION}/lib/x64
#                 $ENV{SDL2_LOCATION}/lib/Release/x64
#                 $ENV{SDL2_LOCATION}/lib/Release MX/x64
#                 $ENV{PROGRAMFILES}/GLEW/lib/x64
#                 ${PROJECT_SOURCE_DIR}/extern/glew/bin
#                 ${PROJECT_SOURCE_DIR}/extern/glew/lib/x64
#                 ${SDL2_LOCATION}/lib
#                 $ENV{SDL2_LOCATION}/lib
#                 $ENV{PROGRAMFILES}/GLEW/lib
#                 ${PROJECT_SOURCE_DIR}/extern/glew/lib
#                 ${SDL2_LOCATION}
#                 $ENV{SDL2_LOCATION}
#                 c:/msys64/mingw32/lib
#             DOC "The SDL2 library")
#     endif()
# elseif(UNIX)
#     find_path( SDL2_INCLUDE_DIR
#         NAMES
#             SDL2/SDL.h
#         PATHS
#             ${SDL2_LOCATION}/include
#             $ENV{SDL2_LOCATION}/include
#             /usr/include/
#             /usr/include/SDL2
#         DOC "The directory where SDL2/sdl.h resides")
#     find_library( SDL2_LIBRARY
#         NAMES
#             SDL2 libSDL2
#         PATHS
#             ${SDL2_LOCATION}/lib
#             $ENV{SDL2_LOCATION}/lib
#             ${SDL2_LOCATION}
#             $ENV{SDL2_LOCATION}
#             /usr/local/lib
#             /usr/lib
#         DOC "The GLEW library")
# endif ()

