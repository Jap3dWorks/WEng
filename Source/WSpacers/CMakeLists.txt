add_executable(WSpacers Source/WSpacers.cpp)

target_include_directories(
    WSpacers 
    PRIVATE 
        Include
        WCore::WCore
        WEngine::WEngine
        WRender::WRender
        WLevel::WLevel
        WImporters::WImporters
        WEngineObjects::WEngineObjects
        WEngineInterfaces::WEngineInterfaces
    PRIVATE 
        PrivateGenerated
)

target_link_libraries(
    WSpacers
    PUBLIC
    PRIVATE
        glfw
        WEngineObjects::WEngineObjects
        WEngineInterfaces::WEngineInterfaces
        WCore::WCore
        WRender::WRender
        WImporters::WImporters
        WEngine::WEngine
        WLevel::WLevel
)

install(
    TARGETS
        WSpacers
    RUNTIME DESTINATION bin
)
install(
    DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/Shaders
        ${CMAKE_CURRENT_SOURCE_DIR}/Assets
    DESTINATION
        Content
)
