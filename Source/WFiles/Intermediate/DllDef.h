#pragma once

#if defined(_WIN32)
    #ifdef WFILES_EXPORTS
        #define WFILES_API __declspec(dllexport)
    #else
        #define WFILES_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #ifdef WFILES_EXPORTS
        #define WFILES_API __attribute__((visibility("default")))
    #else
        #define WFILES_API
    #endif
#else
    #define WFILES_API
#endif
