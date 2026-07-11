# fetch tiniobjloader header
# --------------------------

include(FetchContent)

FetchContent_Declare(
    tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
    GIT_TAG v2.0.0rc13
    GIT_SHALLOW TRUE
)

# Get KTX prebuild libraries
# --------------------------

FetchContent_Declare(
    ktx
    GIT_REPOSITORY https://github.com/KhronosGroup/KTX-Software.git
    GIT_TAG v5.0.0-rc1
    GIT_SHALLOW TRUE
)

# Include fastgltf library
# ------------------------

FetchContent_Declare(
    fastgltf
    GIT_REPOSITORY https://github.com/spnda/fastgltf.git
    GIT_TAG v0.9.0
    GIT_SHALLOW TRUE
)

# FetchContent_Populate(fastgltf)
FetchContent_Populate(tinyobjloader)

FetchContent_MakeAvailable(fastgltf ktx)

add_library(
    WImporter 
    SHARED 
        CompileGenerated/CompileGenerated.cpp
        Source/WImporterRegister.cpp
        Source/WImporterObj.cpp
        Source/WImporterTexture.cpp
        Source/WImporterGltf.cpp
        Source/WLib_stbi.cpp
)

set_target_properties(
    WImporter
    PROPERTIES
        CXX_STANDARD 23
)

target_include_directories(
    WImporter 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PublicGenerated>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/PrivateGenerated
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${tinyobjloader_SOURCE_DIR}
)

target_link_libraries(
    WImporter 
    PRIVATE
        WCore
        WEngineObjects
        WEngineInterfaces
        fastgltf
        ktx
)

include(ShaderUtils)

add_slang_shader_target(
    WImporter_AllShaders
    SHADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content/Shaders
)

# compile shaders first
add_dependencies(WImporter WImporter_AllShaders)

install(
    TARGETS WImporter
    EXPORT WImporterTargets
    LIBRARY DESTINATION lib64
    ARCHIVE DESTINATION lib64
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )

install(
    EXPORT WImporterTargets
    FILE WImporterConfig.cmake
    NAMESPACE WImporter::
    DESTINATION lib/cmake/WImporter
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

install(
    DIRECTORY
      ${CMAKE_CURRENT_SOURCE_DIR}/Content
    DESTINATION
      .
)
