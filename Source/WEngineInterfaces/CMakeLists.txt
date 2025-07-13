add_library(
    WEngineInterfaces
    SHARED
        CompileGenerated/CompileGenerated.cpp
)

target_include_directories(
    WEngineInterfaces
    PUBLIC
        Include
        PublicGenerated
    PRIVATE
        WCore::WCore
        WEngineObjects::WEngineObjects
        PrivateGenerated
        Source
    )

target_link_libraries(
    WEngineInterfaces
    PRIVATE
        WCore::WCore
        WEngineObjects::WEngineObjects
)    

add_library(WEngineInterfaces::WEngineInterfaces ALIAS WEngineInterfaces)

install(
    TARGETS WEngineInterfaces
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)


