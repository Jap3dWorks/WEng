#pragma once

/**
 * Static singleton
 */
template<typename Family, typename T>
struct TSingleton {
    
    // static inline constexpr T Init() {
    //     return T{};
    // }

    // static inline constexpr T value=Init();

    static inline T & Value() {
        static T _v{};
        return _v;
    }
};
