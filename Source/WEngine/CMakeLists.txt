add_library(WEngine SHARED Source/WEngine.cpp)

target_include_directories(
    WEngine 
    PUBLIC 
        Include
    PRIVATE
        Source
        Intermediate
)

add_library(WEngine::WEngine ALIAS WEngine)

install (
    TARGETS WEngine 
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
