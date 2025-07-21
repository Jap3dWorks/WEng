add_library(
    WImporters 
    SHARED 
        CompileGenerated/CompileGenerated.cpp
        Source/WImporters.cpp
        Source/WImporterRegister.cpp
)

find_package(tinyobjloader REQUIRED)

target_include_directories(
    WImporters 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PublicGenerated>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/PrivateGenerated
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${tinyobjloader_INCLUDE_DIRS}
)

target_link_libraries(
    WImporters 
    PRIVATE
        WCore::WCore
        WEngineObjects::WEngineObjects
        WEngineInterfaces::WEngineInterfaces
)

add_library(WImporters::WImporters ALIAS WImporters)

install(
    TARGETS WImporters
    EXPORT WImportersTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )

install(
    EXPORT WImportersTargets
    FILE WImportersConfig.cmake
    NAMESPACE WImporters::
    DESTINATION lib/cmake/WImporters
)

install(
    DIRECTORY Include/
    DESTINATION include
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
)

install(
    DIRECTORY PublicGenerated/
    DESTINATION include
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    WImportersConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion    
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/WImportersConfigVersion.cmake
    DESTINATION lib/cmake/WImporters
)
    
