
include(FindPackageHandleStandardArgs)

if (WIN32)
    find_path( SDL2_INCLUDE_DIR
        NAMES
            SDL2/SDL.h
        PATHS
            C:/msys64/mingw64/include
            ${SDL2_LOCATION}/include
            $ENV{SDL2_LOCATION}/include
            $ENV{PROGRAMFILES}/SDL2/include
            DOC "The directory where SDL2/sdl.h resides" )
    if(ARCH STREQUAL "x86")
      find_library( GLEW_LIBRARY
          NAMES
              glew GLEW glew32s glew32
          PATHS
              ${GLEW_LOCATION}/lib
              ${GLEW_LOCATION}/lib/x86
              ${GLEW_LOCATION}/lib/win32
              ${GLEW_LOCATION}/lib/Release/win32
              ${GLEW_LOCATION}/lib/Release MX/win32
              $ENV{GLEW_LOCATION}/lib
              $ENV{GLEW_LOCATION}/lib/Release/win32
              $ENV{GLEW_LOCATION}/lib/Release MX/win32
              $ENV{GLEW_LOCATION}/lib/x86
              $ENV{GLEW_LOCATION}/lib/win32
              $ENV{PROGRAMFILES}/GLEW/lib
              $ENV{PROGRAMFILES}/GLEW/lib/x86
              $ENV{PROGRAMFILES}/GLEW/lib/win32
              ${PROJECT_SOURCE_DIR}/extern/glew/bin
              ${PROJECT_SOURCE_DIR}/extern/glew/lib
              ${PROJECT_SOURCE_DIR}/extern/glew/lib/x86
              ${PROJECT_SOURCE_DIR}/extern/glew/lib/win32
              ${GLEW_LOCATION}
              $ENV{GLEW_LOCATION}
              DOC "The GLEW library")
    else()
      find_library( GLEW_LIBRARY
          NAMES
              glew GLEW glew32s glew32
          PATHS
              ${GLEW_LOCATION}/lib/x64
              ${GLEW_LOCATION}/lib/Release/x64
              ${GLEW_LOCATION}/lib/Release MX/x64
              $ENV{GLEW_LOCATION}/lib/x64
              $ENV{GLEW_LOCATION}/lib/Release/x64
              $ENV{GLEW_LOCATION}/lib/Release MX/x64
              $ENV{PROGRAMFILES}/GLEW/lib/x64
              ${PROJECT_SOURCE_DIR}/extern/glew/bin
              ${PROJECT_SOURCE_DIR}/extern/glew/lib/x64
              ${GLEW_LOCATION}/lib
              $ENV{GLEW_LOCATION}/lib
              $ENV{PROGRAMFILES}/GLEW/lib
              ${PROJECT_SOURCE_DIR}/extern/glew/lib
              ${GLEW_LOCATION}
              $ENV{GLEW_LOCATION}
              DOC "The GLEW library")
    endif()
endif ()
