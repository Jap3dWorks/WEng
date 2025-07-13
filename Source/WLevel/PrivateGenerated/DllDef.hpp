#pragma once

#if defined(_WIN32)
    #ifdef WLEVEL_EXPORTS
        #define WLEVEL_API __declspec(dllexport)
    #else
        #define WLEVEL_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #ifdef WLEVEL_EXPORTS
        #define WLEVEL_API __attribute__((visibility("default")))
    #else
        #define WLEVEL_API
    #endif
#else
    #define WLEVEL_API
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

#if defined(_WIN32)
    #ifdef WENGINEOBJECTS_EXPORTS
        #define WENGINEOBJECTS_API __declspec(dllexport)
    #else
        #define WENGINEOBJECTS_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #ifdef WENGINEOBJECTS_EXPORTS
        #define WENGINEOBJECTS_API __attribute__((visibility("default")))
    #else
        #define WENGINEOBJECTS_API
    #endif
#else
    #define WENGINEOBJECTS_API
#endif

