add_library(
    WSystem 
    SHARED
        # Source/WSystem.cpp
        Source/WSystemsRegister.cpp
        Source/WSystemsRunner.cpp
)

set_target_properties(
    WSystem
    PROPERTIES
        CXX_STANDARD 23
)

add_dependencies(WSystem ${GENERATE_CODE_TARGET})

target_include_directories(
    WSystem 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PublicGenerated>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/PrivateGenerated
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
 )

target_link_libraries(
    WSystem
    PUBLIC
        WCore
        WObjects
    PRIVATE
    )

install(
    TARGETS WSystem
    LIBRARY DESTINATION lib64
    ARCHIVE DESTINATION lib64
    RUNTIME DESTINATION bin
    INCLUDES DESTINATIoN include
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

