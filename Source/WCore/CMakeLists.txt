
add_library(
    WCore 
    SHARED 
        Compile/Compile.cpp
        Source/WCore.cpp
        Source/WAsset.cpp
        Source/WActor.cpp
        Source/WObjectManager.cpp
)

target_include_directories(
    WCore 
    PUBLIC 
        Include
    PRIVATE
        Source
        Intermediate
)

add_library(WCore::WCore ALIAS WCore)

install(
    TARGETS WCore 
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
