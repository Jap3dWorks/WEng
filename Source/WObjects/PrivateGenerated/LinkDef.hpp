// [[file:../WObjects.org::LinkDef][LinkDef]]
#pragma once

#if defined(_WIN32)
    #ifdef WWINDOW_EXPORTS
        #define WWINDOW_API __declspec(dllexport)
    #else
        #define WWINDOW_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #ifdef WWINDOW_EXPORTS
        #define WWINDOW_API __attribute__((visibility("default")))
    #else
        #define WWINDOW_API
    #endif
#else
    #define WWINDOW_API
#endif


#if defined(_WIN32)
    #ifdef WSPACERS_EXPORTS
        #define WSPACERS_API __declspec(dllexport)
    #else
        #define WSPACERS_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #ifdef WSPACERS_EXPORTS
        #define WSPACERS_API __attribute__((visibility("default")))
    #else
        #define WSPACERS_API
    #endif
#else
    #define WSPACERS_API
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


#if defined(_WIN32)
    #ifdef WOBJECTS_EXPORTS
        #define WOBJECTS_API __declspec(dllexport)
    #else
        #define WOBJECTS_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #ifdef WOBJECTS_EXPORTS
        #define WOBJECTS_API __attribute__((visibility("default")))
    #else
        #define WOBJECTS_API
    #endif
#else
    #define WOBJECTS_API
#endif


#if defined(_WIN32)
    #ifdef WIMPORTER_EXPORTS
        #define WIMPORTER_API __declspec(dllexport)
    #else
        #define WIMPORTER_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #ifdef WIMPORTER_EXPORTS
        #define WIMPORTER_API __attribute__((visibility("default")))
    #else
        #define WIMPORTER_API
    #endif
#else
    #define WIMPORTER_API
#endif


#if defined(_WIN32)
    #ifdef WENGINEINTERFACES_EXPORTS
        #define WENGINEINTERFACES_API __declspec(dllexport)
    #else
        #define WENGINEINTERFACES_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #ifdef WENGINEINTERFACES_EXPORTS
        #define WENGINEINTERFACES_API __attribute__((visibility("default")))
    #else
        #define WENGINEINTERFACES_API
    #endif
#else
    #define WENGINEINTERFACES_API
#endif


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
// LinkDef ends here
