add_library(WRender SHARED Source/WRender.cpp)

target_include_directories(
    WEngine PUBLIC Include
)

add_library(WRender::WRender ALIAS WRender)

install(
    TARGETS WRender
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)