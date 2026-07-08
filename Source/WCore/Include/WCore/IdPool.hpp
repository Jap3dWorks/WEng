#pragma once

#include "WCore/WCoreMacros.hpp"

#include <cassert>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <concepts>


namespace wcr {

    template<std::integral T=std::size_t>
    struct IdRange {

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
    class IdPool {
    public:
    
        IdPool() = default;

        IdPool(std::vector<IdRange<T>> const & in_ranges) :
            free_ranges_(in_ranges) {}

        IdPool(std::vector<IdRange<T>> && in_ranges) :
            free_ranges_(std::move(in_ranges)) {}

        IdPool(const IdPool&) = default;
        IdPool(IdPool&&) noexcept = default;
        IdPool& operator=(const IdPool&) = default;
        IdPool& operator=(IdPool&&) noexcept = default;
        virtual ~IdPool() = default;

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

        void ExtractFromPool(T id)
            {
                std::vector<IdRange<T>> splitted{};
                // splitted.reserve(free_ranges_.max_size());

                bool extracted{false};
    
                for (auto const & r : free_ranges_) {
                    if (r.InRange(id)) {
                        extracted = true;
                
                        IdRange<T> before = {r.first, id-1};
                        IdRange<T> after = {id+1, r.last};
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

                if (extracted) {
                    free_ranges_ = std::move(splitted);
                }
            }

        /**
         * @biref Makes id avaiable.
         */
        void AddToPool(T id) {

            free_ranges_.push_back({id, id});

            std::sort(free_ranges_.begin(), free_ranges_.end(),
                      [](const IdRange<T>& a, const IdRange<T>& b) {
                          return a.first < b.first;
                      });

            std::vector<IdRange<T>> merged;
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
    
        std::vector<IdRange<T>> free_ranges_ { IdRange<T>{} };
    
    };

}
