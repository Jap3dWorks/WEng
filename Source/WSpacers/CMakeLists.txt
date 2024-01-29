add_executable(WSpacers Source/WSpacers.cpp)



target_include_directories(
    WSpacers 
    PRIVATE 
        Include
        WCore::WCore
        WEngine::WEngine
        WRender::WRender    
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