add_library(
    WEngineObjects
    SHARED 
        CompileGenerated/CompileGenerated.cpp
        Source/WAsset.cpp
        Source/WActor.cpp
        Source/WComponent.cpp
        Source/WTextureAsset.cpp
        Source/WObjectDb.cpp
        Source/WAssetDb.cpp
        Source/WActorComponentDb.cpp
)

target_include_directories(
    WEngineObjects 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PublicGenerated>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/PrivateGenerated
    )

target_link_libraries(
    WEngineObjects
    PRIVATE
        WCore::WCore
)    

add_library(WEngineObjects::WEngineObjects ALIAS WEngineObjects)

install(
    TARGETS WEngineObjects
    EXPORT WEngineObjectsTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Generate and install config files
install(
    EXPORT WEngineObjectsTargets
    FILE WEngineObjectsConfig.cmake
    NAMESPACE WEngineObjects::
    DESTINATION lib/cmake/WEngineObjects
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
    WEngineObjectsConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/WEngineObjectsConfigVersion.cmake
    DESTINATION lib/cmake/WEngineObjects
)

# Unittest
# --------

if (DEFINED WUNITTEST OR DEFINED ENV{WUNITTEST})

    message("BUILD WEngineObjects unittests.")

    find_package(Catch2 2 REQUIRED)

    add_executable(
        WEngineObjects_unittest
        unittest/WEngineObjects_unittest.cpp
    )

    target_include_directories(
        WEngineObjects_unittest
        PUBLIC 
            Include
            PublicGenerated
        PRIVATE
            Source
            PrivateGenerated
            Catch2::Catch2
        )

    target_link_libraries(
        WEngineObjects_unittest
        PRIVATE
            Catch2::Catch2
            WCore::WCore
            WEngineObjects::WEngineObjects
    )

    install(
        TARGETS
        WEngineObjects_unittest
        RUNTIME DESTINATION bin
    )

else()

    message("Exclude WEngineObjects unittests build.")

endif()
