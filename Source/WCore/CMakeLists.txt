
add_library(WCore SHARED Source/WCore.cpp)

target_include_directories(WCore PUBLIC Include)
target_link_libraries(
    WCore 
)

install(TARGETS WCore DESTINATION bin)
