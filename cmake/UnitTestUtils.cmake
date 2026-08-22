include_guard()

include(FetchContent)

function(download_catch2)
    FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.15.3
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(catch2)
endfunction()
