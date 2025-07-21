add_library(
    WEngineInterfaces INTERFACE
)

target_include_directories(
    WEngineInterfaces INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/PublicGenerated>
        $<INSTALL_INTERFACE:include>
)

add_library(WEngineInterfaces::WEngineInterfaces ALIAS WEngineInterfaces)

install(
    TARGETS WEngineInterfaces
    EXPORT WEngineInterfacesTargets
    INCLUDES DESTINATION include

    # LIBRARY DESTINATION lib
    # ARCHIVE DESTINATION lib
    # RUNTIME DESTINATION bin
)

install(
    DIRECTORY Include/
    DESTINATION include
    FILES_MATCHING
    PATTERN "*.hpp"
    PATTERN "*.h"
)

# Install config files
install(
    EXPORT WEngineInterfacesTargets
    FILE WEngineInterfacesConfig.cmake
    NAMESPACE WEngineInterfaces::
    DESTINATION lib/cmake/WEngineInterfaces
)

