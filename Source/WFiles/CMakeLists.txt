
add_library(WFiles SHARED Source/WFiles.cpp)

target_include_directories(
    WFiles 
    PUBLIC 
        Include
)

add_library(WFiles::WFiles ALIAS WFiles)

install(
    TARGETS WFiles 
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
