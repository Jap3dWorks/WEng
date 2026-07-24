add_library(
    WObjects
    SHARED 
        CompileGenerated/CompileGenerated.cpp
        Source/WAsset.cpp
        Source/WEntity.cpp
        Source/WComponent.cpp
        Source/WObjectDb.cpp
        Source/WAssetDb.cpp
        Source/WEntityComponentDb.cpp
        Source/Level.cpp
)

set_target_properties(
    WObjects
    PROPERTIES
        CXX_STANDARD 23
)

target_include_directories(
    WObjects 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PublicGenerated>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/PrivateGenerated
    )

target_link_libraries(
    WObjects
    PRIVATE
        WCore
)    

install(
    TARGETS WObjects
    EXPORT WObjectsTargets
    LIBRARY DESTINATION lib64
    ARCHIVE DESTINATION lib64
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Generate and install config files
install(
    EXPORT WObjectsTargets
    FILE WObjectsConfig.cmake
    NAMESPACE WObjects::
    DESTINATION lib/cmake/WObjects
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

# # Version config file
# include(CMakePackageConfigHelpers)
# write_basic_package_version_file(
#     WObjectsConfigVersion.cmake
#     VERSION ${PROJECT_VERSION}
#     COMPATIBILITY SameMajorVersion
# )

# install(
#     FILES ${CMAKE_CURRENT_BINARY_DIR}/WObjectsConfigVersion.cmake
#     DESTINATION lib64/cmake/WObjects
# )

# Unittest
# --------
if (DEFINED WUNITTEST)

    message("BUILD WObjects unittests.")

    find_package(Catch2 2 REQUIRED)

    add_executable(
        WObjects_unittest
        unittest/WObjects_unittest.cpp
    )

    set_target_properties(
        WObjects_unittest
        PROPERTIES
        CXX_STANDARD 23
    )

    target_include_directories(
        WObjects_unittest
        PUBLIC 
            Include
            PublicGenerated
        PRIVATE
            Source
            PrivateGenerated
            Catch2::Catch2
        )

    target_link_libraries(
        WObjects_unittest
        PRIVATE
            Catch2::Catch2
            WCore
            WObjects
    )

    install(
        TARGETS
        WObjects_unittest
        RUNTIME DESTINATION bin
    )

else()

    message("Exclude WObjects unittests build.")

endif()
