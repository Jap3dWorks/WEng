#pragma once

#include <glm/gtc/constants.hpp>
#include <type_traits>
#include <cmath>

namespace wmath::numerical {

    template<typename T=float>
    inline constexpr T PI = glm::pi<T>();

    template<typename T> requires requires {
        std::is_floating_point_v<T>;
    }
    inline constexpr bool AreEqual(T a, T b, T epsilon=2.5E-06) {
        return std::abs(a - b) <= epsilon; 
    }

}

    
