add_library(
    WLevel
    SHARED
        CompileGenerated/CompileGenerated.cpp
        Source/WLevel.cpp
    )

target_include_directories(
    WLevel
    PUBLIC
        Include
        PublicGenerated
    PRIVATE
        PrivateGenerated
        WCore::WCore
        WEngineObjects::WEngineObjects
        WEngineInterfaces::WEngineInterfaces
        Source
)    

target_link_libraries(
    WLevel
    PRIVATE
    WCore::WCore
    WEngineObjects::WEngineObjects
    WEngineInterfaces::WEngineInterfaces
)

add_library(WLevel::WLevel ALIAS WLevel)

install(
    TARGETS WLevel
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
