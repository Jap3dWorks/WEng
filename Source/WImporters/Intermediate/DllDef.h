#pragma once

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
