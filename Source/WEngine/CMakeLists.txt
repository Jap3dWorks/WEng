add_library(
    WEngine 
    SHARED
        CompileGenerated/CompileGenerated.cpp
        Source/WEngine.cpp
        Source/WInputMappingRegister.cpp
)

target_include_directories(
    WEngine 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PublicGenerated>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/PrivateGenerated
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
 )

target_link_libraries(
    WEngine
    PRIVATE
        WCore::WCore
        WEngineObjects::WEngineObjects
        WImporters::WImporters
        WRender::WRender
        WEngineInterfaces::WEngineInterfaces
        WLevel::WLevel
)

add_library(WEngine::WEngine ALIAS WEngine)

install(
    TARGETS WEngine
    EXPORT WEngineTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATIoN include
)

install(
    EXPORT WEngineTargets
    FILE WEngineConfig.cmake
    NAMESPACE WEngine::
    DESTINATION lib/cmake/WEngine
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

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    WEngineConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion    
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/WEngineConfigVersion.cmake
    DESTINATION lib/cmake/WEngine
)

