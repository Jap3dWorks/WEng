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
        Source
        PrivateGenerated
)

add_library(WEngine::WEngine ALIAS WEngine)

install (
    TARGETS WEngine 
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
