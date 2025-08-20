add_library(
    WCore 
    SHARED 
        CompileGenerated/CompileGenerated.cpp
)

find_package(glm REQUIRED)

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
    PRIVATE
        glm::glm
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

# Unittest
# --------

if (DEFINED WUNITTEST OR DEFINED ENV{WUNITTEST})

    message("BUILD WCore unittests.")

    find_package(Catch2 2 REQUIRED)

    add_executable(
        WCore_unittest
        unittest/WCore_unittest.cpp
    )

    target_include_directories(
        WCore_unittest
        PUBLIC
            Inlcude
            PublicGenerated
        PRIVATE
            Source
            PrivateGenerated
            Catch2:Catch2
        )

    target_link_libraries(
        WCore_unittest
        PRIVATE
            Catch2::Catch2
            WCore::WCore
        )

    install(
        TARGETS
        WCore_unittest
        RUNTIME DESTINATION bin
    )

else()

    message("Exclude WCore unittests build.")

endif()
