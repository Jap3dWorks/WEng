add_library(
    WEngine 
    SHARED
        CompileGenerated/CompileGenerated.cpp
        Source/WEngine.cpp
)

target_include_directories(
    WEngine 
    PUBLIC 
        Include
        PublicGenerated
    PRIVATE
        WCore::WCore
        WEngineObjects::WEngineObjects
        WEngineInterfaces::WEngineInterfaces
        WRender::WRender
        WImporters::WImporters
        WLevel::WLevel
        PrivateGenerated
        Source
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

install (
    TARGETS WEngine 
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
