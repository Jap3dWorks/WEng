add_library(
    WLevel
    SHARED
        CompileGenerated/CompileGenerated.cpp
        Source/WLevel.cpp
        Source/WLevelDb.cpp
    )

target_include_directories(
    WLevel
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PublicGenerated>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/PrivateGenerated
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)    

target_link_libraries(
    WLevel
    PRIVATE
        WCore::WCore
        WEngineObjects::WEngineObjects
        WEngineInterfaces::WEngineInterfaces
)

add_library(WLevel::WLevel ALIAS WLevel)

install(
    TARGETS WLevel
    EXPORT WLevelTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Generate and install config files
install(
    EXPORT WLevelTargets
    FILE WLevelConfig.cmake
    NAMESPACE WLevel::
    DESTINATION lib/cmake/WLevel
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
    WLevelConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/WLevelConfigVersion.cmake
    DESTINATION lib/cmake/WLevel
)
