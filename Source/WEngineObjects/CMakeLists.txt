
add_library(
    WEngineObjects
    SHARED 
        CompileGenerated/CompileGenerated.cpp
        Source/WAsset.cpp
        Source/WActor.cpp
        Source/WObjectManager.cpp
        Source/WComponent.cpp
        Source/WTextureAsset.cpp
)

target_include_directories(
    WEngineObjects 
    PUBLIC 
        Include
        PublicGenerated
    PRIVATE
        Source
        PrivateGenerated
        WCore::WCore
    )

target_link_libraries(
    WEngineObjects
    PRIVATE
        WCore::WCore
)    

add_library(WEngineObjects::WEngineObjects ALIAS WEngineObjects)

install(
    TARGETS WEngineObjects 
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
