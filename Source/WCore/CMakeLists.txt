
add_library(WCore SHARED Source/WCore.cpp)

target_include_directories(WCore PUBLIC Include)
# target_link_libraries(
#     WCore 
# )

add_library(WCore::WCore ALIAS WCore)

install(
    TARGETS WCore 
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
