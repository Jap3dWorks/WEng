#pragma once

#include "WCore/WCoreMacros.hpp"

#include <cassert>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <concepts>

template<std::integral T=std::size_t>
struct WIdRange {

    T first{1};
    T last{ std::numeric_limits<T>::max() };

    WNODISCARD constexpr bool InRange(T in_value) const noexcept {
        return in_value >= first && in_value <= last;
    }

    WNODISCARD constexpr bool IsValid() const noexcept {
        return first <= last;
    }
};

template<std::integral T=std::size_t>
class WIdPool {
public:
    
    T Generate() {
        if (free_ranges_.empty()) {
            throw std::runtime_error(
                "Ges Id Pool is empty, no more DTId's can be generated!"
                );
        }
        
        auto& front = free_ranges_.front();
        auto result = front.first++;
        if (front.first > front.last) {
            // Range exhausted
            free_ranges_.erase(free_ranges_.begin());
        }
        return result;
    }

    void Reserve(T id)
    {
        std::vector<WIdRange<T>> splitted;
        splitted.reserve(free_ranges_.max_size());

        bool reserved{false};
    
        for (auto const & r : free_ranges_) {
            if (r.InRange(id)) {
                reserved = true;
                
                WIdRange<T> before = {r.first, id-1};
                WIdRange<T> after = {id+1, r.last};
                if (before.IsValid()) {
                    splitted.push_back(r);
                }
                if (after.IsValid()) {
                    splitted.push_back(r);
                }
            }
            else {
                splitted.push_back(r);
            }
        }

        if (reserved) {
            free_ranges_ = std::move(splitted);
        }
    }

    /**
     * @biref Makes id avaiable.
     */
    void Release(T id) {

        free_ranges_.push_back({id, id});

        std::sort(free_ranges_.begin(), free_ranges_.end(),
                  [](const WIdRange<T>& a, const WIdRange<T>& b) {
                      return a.first < b.first;
                  });

        std::vector<WIdRange<T>> merged;
        for (auto const & r : free_ranges_) {
            if (merged.empty()) {
                merged.push_back(r);
            } else {
                auto& last = merged.back();
                if (r.first <= last.last + 1) {
                    last.last = std::max(last.last, r.last);
                } else {
                    merged.push_back(r);
                }
            }
        }
        free_ranges_ = std::move(merged);
    }

    void Clear() {
        free_ranges_ = {};
    }
   
private:
    
    std::vector<WIdRange<T>> free_ranges_{{}};
    
};

