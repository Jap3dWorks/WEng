add_library(
    WMath
    INTERFACE
)

set_target_properties(
    WMath
    PROPERTIES
        CXX_STANDARD 23
)

add_dependencies(WMath ${GENERATE_CODE_TARGET})

target_include_directories(
    WMath
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PublicGenerated>
        $<INSTALL_INTERFACE:include>
    )

target_link_libraries(
    WMath
    INTERFACE
        glm
)
    
install(
    TARGETS WMath
    LIBRARY DESTINATION lib64
    ARCHIVE DESTINATION lib64
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
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

# unittest
if (DEFINED WUNITTEST)

    message("BUILD WMath unittests.")

    include(UnitTestUtils)
    download_catch2()

    add_executable(
        WMath_unittest
        unittest/WMath_unittest.cpp
    )

    set_target_properties(
        WMath_unittest
        PROPERTIES
        CXX_STANDARD 23
    )

    target_include_directories(
        WMath_unittest
        PUBLIC
            Inlcude
            PublicGenerated
        PRIVATE
            Source
            PrivateGenerated
        )

    target_link_libraries(
        WMath_unittest
        PRIVATE
            Catch2::Catch2WithMain
            WMath
            WCore
        )

    install(
        TARGETS
        WMath_unittest
        RUNTIME DESTINATION bin
    )

else()

    message("Exclude WMath unittests build.")

endif()
