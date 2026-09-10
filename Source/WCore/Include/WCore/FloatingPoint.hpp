#pragma once

#include <cmath>
#include <type_traits>

namespace wcr::fpoint{

    template<typename T> requires requires {
        std::is_floating_point_v<T>;
    }
    inline constexpr bool AreEqual(T a, T b, T epsilon=2.5E-06) {
        return std::abs(a - b) <= epsilon; 
    }

}
