cmake_minimum_required (VERSION 3.20)

project(WEng VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_VERBOSE_MAKEFILE on)

# if (EMSCRIPTEN)
#     message("EMSCRIPTEN build process.")
#     message("Add EMSCRIPTEN compile flags.")
#     if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#       set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g4 -sASSERTIONS=1")
#       set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g4 -sASSERTIONS=1")
#     endif()
#     set(CMAKE_EXECUTABLE_SUFFIX ".html")
# endif ()

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
  message(STATUS "Platform is Windows.")
  add_compile_options(-fdeclspec)
endif ()

if (CMAKE_GENERATOR MATCHES "Visual Studio.*")
else()
endif()

macro(FIND_CMAKE_LISTS result curdir)
  FILE(GLOB_RECURSE children ABSOLUTE ${curdir} ${curdir}/*/CMakeLists.txt)
  SET(dirlist "")
  foreach(child ${children})
    message(STATUS "CMakeLists.txt file found, ${child}.")
    cmake_path(GET child PARENT_PATH subdir)
    LIST(APPEND dirlist ${subdir})
  endforeach()
  SET(${result} ${dirlist})
endmacro()

# Collect project modules
message(STATUS "Start collecting Modules.")
find_cmake_lists(SUBDIRS "${PROJECT_SOURCE_DIR}/Source")

foreach(subdir ${SUBDIRS})
  message(STATUS "Add subdirectory, ${subdir}.")
  add_subdirectory(${subdir})
endforeach()


