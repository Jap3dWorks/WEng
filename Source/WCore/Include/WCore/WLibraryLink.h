#pragma once

#if defined(__GNUC__) || defined(__clang__)
    #define HIDDEN __attribute__((visibility("hidden")))
#else
    #define HIDDEN
#endif

