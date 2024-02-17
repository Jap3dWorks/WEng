
add_library(
    WImporters 
    SHARED 
        CompileGenerated/CompileGenerated.cpp
        Source/WImporters.cpp
)

target_include_directories(
    WImporters 
    PUBLIC 
        Include
        PublicGenerated
    PRIVATE
        WCore::WCore
        PrivateGenerated
        Source
)

target_link_libraries(
    WImporters 
    PRIVATE
        WCore::WCore
)

add_library(WImporters::WImporters ALIAS WImporters)

install(
    TARGETS WImporters 
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
