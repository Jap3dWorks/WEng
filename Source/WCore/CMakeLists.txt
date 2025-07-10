
add_library(
    WCore 
    SHARED 
        CompileGenerated/CompileGenerated.cpp
)

target_include_directories(
    WCore 
    PUBLIC 
        Include
        PublicGenerated
    PRIVATE
        Source
        PrivateGenerated
)

add_library(WCore::WCore ALIAS WCore)

install(
    TARGETS WCore 
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
