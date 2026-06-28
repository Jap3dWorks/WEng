cmake_minimum_required (VERSION 3.20)

project(WEng VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_VERBOSE_MAKEFILE on)

# Enable Logging
# TODO do not use ENV
if (DEFINED WENG_WLOG_ENABLE OR DEFINED ENV{WENG_WLOG_ENABLE})
    add_compile_definitions(WENG_WLOG_ENABLE=1)
endif()

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
  message(STATUS "Platform is Windows.")
  add_compile_options(-fdeclspec)
endif ()

if (CMAKE_GENERATOR MATCHES "Visual Studio.*")
else()
endif()

# fetch tiniobjloader header
# --------------------------
include(FetchContent)

FetchContent_Declare(
    tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
    GIT_TAG v2.0.0rc13
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(tinyobjloader)

# Get slang prebuild tools
# ------------------------
include(SlangSetup)
download_slang()


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


