add_library(
    WCore 
    SHARED 
        CompileGenerated/CompileGenerated.cpp
)

set_target_properties(
    WCore
    PROPERTIES
        CXX_STANDARD 23
)

add_dependencies(WCore ${GENERATE_CODE_TARGET})

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

target_link_libraries(
    WCore
    PUBLIC
        glm::glm
)

install(
    TARGETS WCore
    # EXPORT WCoreTargets
    LIBRARY DESTINATION lib64
    ARCHIVE DESTINATION lib64
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# # Generate and install config files
# install(
#     EXPORT WCoreTargets
#     FILE WCoreConfig.cmake
#     NAMESPACE WCore::
#     DESTINATION lib/cmake/WCore
# )


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
#     WCoreConfigVersion.cmake
#     VERSION ${PROJECT_VERSION}
#     COMPATIBILITY SameMajorVersion
# )

# install(
#     FILES ${CMAKE_CURRENT_BINARY_DIR}/WCoreConfigVersion.cmake
#     DESTINATION lib64/cmake/WCore
# )

# Unittest
# --------

if (DEFINED WUNITTEST)

    message("BUILD WCore unittests.")

    include(UnitTestUtils)
    download_catch2()

    add_executable(
        WCore_unittest
        unittest/WCore_unittest.cpp
    )

    set_target_properties(
        WCore_unittest
        PROPERTIES
        CXX_STANDARD 23
    )

    target_include_directories(
        WCore_unittest
        PUBLIC
            Inlcude
            PublicGenerated
        PRIVATE
            Source
            PrivateGenerated
        )

    target_link_libraries(
        WCore_unittest
        PRIVATE
            Catch2::Catch2WithMain
            WCore
        )

    install(
        TARGETS
        WCore_unittest
        RUNTIME DESTINATION bin
    )

else()

    message("Exclude WCore unittests build.")

endif()
