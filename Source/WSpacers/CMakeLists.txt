add_executable(
    WSpacers
    Source/WSpacers.cpp
    CompileGenerated/CompileGenerated.cpp
)

set_target_properties(
    WSpacers
    PROPERTIES
        CXX_STANDARD 23
)

target_include_directories(
    WSpacers 
    PRIVATE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PublicGenerated>
    PRIVATE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PrivateGenerated>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
)

target_link_libraries(
    WSpacers
    PUBLIC
    PRIVATE
        WCore
        WEngineObjects
        WEngineInterfaces
        WRender
        WImporter
        WEngine
        WLevel
)

install(
    TARGETS WSpacers
    RUNTIME DESTINATION bin
)

install(
    DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/Content
    DESTINATION
        .
)

