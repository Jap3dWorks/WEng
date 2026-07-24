add_library(
    WInterfaces INTERFACE
)

set_target_properties(
    WInterfaces
    PROPERTIES
        CXX_STANDARD 23
)

target_include_directories(
    WInterfaces INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PublicGenerated>
        $<INSTALL_INTERFACE:include>
)

install(
    TARGETS WInterfaces
    EXPORT WInterfacesTargets
    INCLUDES DESTINATION include
)

# Install config files
install(
    EXPORT WInterfacesTargets
    FILE WInterfacesConfig.cmake
    NAMESPACE WInterfaces::
    DESTINATION lib/cmake/WInterfaces
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

# include(CMakePackageConfigHelpers)
# write_basic_package_version_file(
#     WInterfacesConfigVersion.cmake
#     VERSION ${PROJECT_VERSION}
#     COMPATIBILITY SameMajorVersion    
# )

# install(
#     FILES ${CMAKE_CURRENT_BINARY_DIR}/WInterfacesConfigVersion.cmake
#     DESTINATION lib64/cmake/WInterfaces
# )

