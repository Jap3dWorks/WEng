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

#if defined(_WIN32)
    #ifdef WIMPORTERS_EXPORTS
        #define WIMPORTERS_API __declspec(dllexport)
    #else
        #define WIMPORTERS_API __declspec(dllimport)
    #endif

#elif defined(__GNUC__) || defined(__clang__)
    #ifdef WIMPORTERS_EXPORTS
        #define WIMPORTERS_API __attribute__((visibility("default")))
    #else
        #define WIMPORTERS_API
    #endif
#else
    #define WIMPORTERS_API
#endif

#if defined(_WIN32)
    #ifdef WRENDER_EXPORTS
        #define WRENDER_API __declspec(dllexport)
    #else
        #define WRENDER_API __declspec(dllimport)
    #endif

#elif defined(__GNUC__) || defined(__clang__)
    #ifdef WRENDER_EXPORTS
        #define WRENDER_API __attribute__((visibility("default")))
    #else
        #define WRENDER_API
    #endif
#else
    #define WRENDER_API
#endif
