
add_library(
    WFiles 
    SHARED 
        Compile/Compile.cpp
        Source/WFiles.cpp
)

target_include_directories(
    WFiles 
    PUBLIC 
        Include
    PRIVATE
        Intermediate
        Source
        WCore::WCore
)

target_link_libraries(
    WFiles 
    PRIVATE
        WCore::WCore
)

add_library(WFiles::WFiles ALIAS WFiles)

install(
    TARGETS WFiles 
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
