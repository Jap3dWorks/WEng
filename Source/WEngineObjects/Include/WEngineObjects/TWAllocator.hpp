#pragma once

#include <memory>
#include <type_traits>
#include "WCore/TFunction.hpp"


template<typename T>
class TWAllocator {
public:
    
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::true_type;

    constexpr TWAllocator() noexcept {}

    TWAllocator(const TWAllocator & other) noexcept {}

    TWAllocator(TWAllocator && other) noexcept {}

    constexpr ~TWAllocator(){}

    constexpr pointer allocate(std::size_t n) {}

    constexpr void deallocate (pointer p, std::size_t n) {}

private:

    TFunction<void(pointer, pointer)> allocate_fn_;


};

template< class T1, class T2 >
constexpr bool operator==( const TWAllocator<T1>& lhs, const TWAllocator<T2>& rhs ) noexcept
{
    return std::is_same_v<T1, T2>;
}
