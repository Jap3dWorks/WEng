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
        PrivateGenerated
        Source
)

add_library(WEngineInterfaces::WEngineInterfaces ALIAS WEngine)

install(
    TARGETS WEngineInterfaces
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)


