add_executable(WSpacers Source/WSpacers.cpp)


target_include_directories(
    WSpacers 
    PRIVATE 
        Include
        WCore::WCore
        WEngine::WEngine
        WRender::WRender
    PRIVATE 
        Intermediate
)

target_link_libraries(
    WSpacers 
    PUBLIC 
        WCore::WCore
        WEngine::WEngine
        WRender::WRender
)

install(
    TARGETS WSpacers
    RUNTIME DESTINATION bin
)
install(
    DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/Shaders
    ${CMAKE_CURRENT_SOURCE_DIR}/Assets
    DESTINATION bin/WVulkanTest_Content
)