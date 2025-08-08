add_executable(
    WSpacers
    Source/WSpacers.cpp
    CompileGenerated/CompileGenerated.cpp
)

target_include_directories(
    WSpacers 
    PRIVATE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PublicGenerated>
    PRIVATE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PrivateGenerated>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
)

target_link_libraries(
    WSpacers
    PUBLIC
    PRIVATE
        WCore::WCore
        WEngineObjects::WEngineObjects
        WEngineInterfaces::WEngineInterfaces
        WRender::WRender
        WImporters::WImporters
        WEngine::WEngine
        WLevel::WLevel
        
)

install(
    TARGETS WSpacers
    RUNTIME DESTINATION bin
)
install(
    DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/Shaders
        ${CMAKE_CURRENT_SOURCE_DIR}/Assets
    DESTINATION
        Content
)
