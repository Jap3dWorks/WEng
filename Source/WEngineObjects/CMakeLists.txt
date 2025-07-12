add_library(
    WEngineObjects
    SHARED 
        CompileGenerated/CompileGenerated.cpp
        Source/WAsset.cpp
        Source/WActor.cpp
        Source/WComponent.cpp
        Source/WTextureAsset.cpp
)

target_include_directories(
    WEngineObjects 
    PUBLIC 
        Include
        PublicGenerated
    PRIVATE
        Source
        PrivateGenerated
        WCore::WCore
    )

target_link_libraries(
    WEngineObjects
    PRIVATE
        WCore::WCore
)    

add_library(WEngineObjects::WEngineObjects ALIAS WEngineObjects)

install(
    TARGETS
    WEngineObjects 
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Unittest
# --------

if (DEFINED WUNITTEST OR DEFINED ENV{WUNITTEST})

    message("BUILD WEngineObjects unittests.")

    find_package(Catch2 2 REQUIRED)

    add_executable(
        WEngineObjects_unittest
        unittest/WEngineObjects_unittest.cpp
    )

    target_include_directories(
        WEngineObjects_unittest
        PUBLIC 
            Include
            PublicGenerated
        PRIVATE
            Source
            PrivateGenerated
            WCore::WCore
            Catch2::Catch2
        )

    target_link_libraries(
        WEngineObjects_unittest
        PRIVATE
            Catch2::Catch2
            WCore::WCore
    )

    install(
        TARGETS
        WEngineObjects_unittest
        RUNTIME DESTINATION bin
    )

else()

    message("Exclude WEngineObjects unittests build.")

endif()
