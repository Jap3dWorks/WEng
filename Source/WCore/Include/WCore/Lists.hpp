#pragma once

#include <cstdint>
#include <span>


namespace wcr::lists {

    /**
     * Apply a permutation list to sort a list with the permutation data.
     * Permutation numbers should start at 0,
     * Permutation list should not contain duplicated indexes.
     */
    template<typename T>
    void ApplyPermutation(
        T & out_list,
        std::span<std::uint32_t> permutation
        ) {
        for(std::uint32_t i=0; i<permutation.size(); i++) {
            auto idx = permutation[i];
            if (idx <= i) continue;

            std::swap(out_list[i], out_list[idx]);
        }
    }
}
