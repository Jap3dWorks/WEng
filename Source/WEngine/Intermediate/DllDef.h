#pragma once

#if defined(_WIN32)
    #ifdef WENGINE_EXPORTS
        #define WENGINE_API __declspec(dllexport)
    #else
        #define WENGINE_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #ifdef WENGINE_EXPORTS
        #define WENGINE_API __attribute__((visibility("default")))
    #else
        #define WENGINE_API
    #endif
#else
    #define WENGINE_API
#endif
