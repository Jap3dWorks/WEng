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

#if defined(_WIN32)
    #ifdef WCORE_EXPORTS
        #define WCORE_API __declspec(dllexport)
    #else
        #define WCORE_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #ifdef WCORE_EXPORTS
        #define WCORE_API __attribute__((visibility("default")))
    #else
        #define WCORE_API
    #endif
#else
    #define WCORE_API
#endif

