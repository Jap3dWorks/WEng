add_library(
    WCore 
    SHARED 
        CompileGenerated/CompileGenerated.cpp
)

target_include_directories(
    WCore 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PublicGenerated>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/PrivateGenerated
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

add_library(WCore::WCore ALIAS WCore)

install(
    TARGETS WCore
    EXPORT WCoreTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Generate and install config files
install(
    EXPORT WCoreTargets
    FILE WCoreConfig.cmake
    NAMESPACE WCore::
    DESTINATION lib/cmake/WCore
)


# Install public headers
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


# Version config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    WCoreConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/WCoreConfigVersion.cmake
    DESTINATION lib/cmake/WCore
)


