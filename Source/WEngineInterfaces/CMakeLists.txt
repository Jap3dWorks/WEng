add_library(
    WEngineInterfaces INTERFACE
)

target_include_directories(
    WEngineInterfaces INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PublicGenerated>
        $<INSTALL_INTERFACE:include>
)

add_library(WEngineInterfaces::WEngineInterfaces ALIAS WEngineInterfaces)

install(
    TARGETS WEngineInterfaces
    EXPORT WEngineInterfacesTargets
    INCLUDES DESTINATION include
)

# Install config files
install(
    EXPORT WEngineInterfacesTargets
    FILE WEngineInterfacesConfig.cmake
    NAMESPACE WEngineInterfaces::
    DESTINATION lib/cmake/WEngineInterfaces
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
    WEngineInterfacesConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion    
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/WEngineInterfacesConfigVersion.cmake
    DESTINATION lib/cmake/WEngineInterfaces
)

