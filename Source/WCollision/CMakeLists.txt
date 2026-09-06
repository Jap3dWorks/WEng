add_library(
    WCollision
    SHARED
        CompileGenerated/CompileGenerated.cpp
)

set_target_properties(
    WCollision
    PROPERTIES
        CXX_STANDARD 23
)

add_dependencies(WCollision ${GENERATE_CODE_TARGET})

target_include_directories(
    WCollision
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PublicGenerated>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/PrivateGenerated
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(
    WCollision
    PUBLIC
        glm
        WCore
    PRIVATE
)

install(
    TARGETS WCollision
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
    
