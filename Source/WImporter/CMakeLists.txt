add_library(
    WImporter 
    SHARED 
        CompileGenerated/CompileGenerated.cpp
        Source/WImporter.cpp
        Source/WImporterRegister.cpp
)

find_package(tinyobjloader REQUIRED)

target_include_directories(
    WImporter 
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
    WImporter 
    PRIVATE
        WCore::WCore
        WEngineObjects::WEngineObjects
        WEngineInterfaces::WEngineInterfaces
)

add_library(WImporter::WImporter ALIAS WImporter)

install(
    TARGETS WImporter
    EXPORT WImporterTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )

install(
    EXPORT WImporterTargets
    FILE WImporterConfig.cmake
    NAMESPACE WImporter::
    DESTINATION lib/cmake/WImporter
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
    WImporterConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion    
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/WImporterConfigVersion.cmake
    DESTINATION lib/cmake/WImporter
)
    
