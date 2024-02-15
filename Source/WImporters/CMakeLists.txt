
add_library(
    WImporters 
    SHARED 
        Compile/Compile.cpp
        Source/WImporters.cpp
)

target_include_directories(
    WImporters 
    PUBLIC 
        Include
    PRIVATE
        WCore::WCore
        Intermediate
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
