#pragma once

// #include <memory>
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

    using DeallocateFunction = TFunction<void(pointer, std::size_t)>;
    using AllocateFunction = TFunction<void(pointer _pptr, std::size_t _pn, pointer _nptr, std::size_t _nn)>;

    constexpr TWAllocator() noexcept :
        allocate_fn_([](pointer, std::size_t, pointer, std::size_t){}),
        deallocate_fn_([](pointer, std::size_t){}),
        pptr_(nullptr)
        {}

    TWAllocator(const TWAllocator & other) :
        allocate_fn_(other.allocate_fn_),
        deallocate_fn_(other.deallocate_fn_),
        pptr_(nullptr)  // Do not copy the previous pointer.
        {}

    TWAllocator(TWAllocator && other) noexcept :
        allocate_fn_(std::move(other.allocate_fn_)),
        deallocate_fn_(std::move(other.deallocate_fn_)),
        pptr_(std::move(other))
        {
            other.pptr_ = nullptr;
        }

    constexpr ~TWAllocator() {
        pptr_ = nullptr;
    }

    constexpr pointer allocate(std::size_t n) {
        pointer p = new T[n];
        
        allocate_fn_(pptr_, pn_, p, n);

        pptr_=p;
        pn_=n;

        return p;
    }

    constexpr void deallocate (pointer p, std::size_t n) {
        deallocate_fn_(p, n);
        delete[] p;
    }

    void SetAllocateFn(const AllocateFunction & in_fn) {
        allocate_fn_ = in_fn;
    }

    void SetAllocateFn(AllocateFunction && in_fn) noexcept {
        allocate_fn_ = std::move(in_fn);
    }

    void SetDeallocateFn(const DeallocateFunction & in_fn) {
        deallocate_fn_ = in_fn;
    }

    void SetDeallocateFn(DeallocateFunction && in_fn) noexcept {
        deallocate_fn_ = std::move(in_fn);
    }

private:

    AllocateFunction allocate_fn_;
    DeallocateFunction deallocate_fn_;

    pointer pptr_;
    std::size_t pn_;

};

template< class T1, class T2 >
constexpr bool operator==( const TWAllocator<T1>& lhs, const TWAllocator<T2>& rhs ) noexcept
{
    return std::is_same_v<T1, T2>;
}


