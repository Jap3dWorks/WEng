
add_library(
    WImporters 
    SHARED 
        CompileGenerated/CompileGenerated.cpp
        Source/WImporters.cpp
        Source/WImporterRegister.cpp
)

find_package(tinyobjloader REQUIRED)

target_include_directories(
    WImporters 
    PUBLIC 
        Include
        PublicGenerated
    PRIVATE
        WCore::WCore
        WEngineObjects::WEngineObjects
        # WEngineInterfaces::WEngineInterfaces
        ../WEngineInterfaces/Include
        PrivateGenerated
        Source
        ${tinyobjloader_INCLUDE_DIRS}
)

target_link_libraries(
    WImporters 
    PRIVATE
        WCore::WCore
        WEngineObjects::WEngineObjects
)

add_library(WImporters::WImporters ALIAS WImporters)

install(
    TARGETS WImporters 
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
