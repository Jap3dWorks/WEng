#pragma once

/**
 * Constexpr static singleton
 */
template<typename Family, typename T>
struct TSingleton {
    static inline constexpr T Init() {
        return T{};
    }

    static inline constexpr T value=Init();
};
