#pragma once

#include <type_traits>
#include "WCore/WConcepts.hpp"

template<typename T,
         CCallable<void, T*, std::size_t, T*, std::size_t>  AllocateFn,
         CCallable<void, T*, std::size_t> DeallocateFn>
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

    constexpr TWAllocator() noexcept :
        allocate_fn_(),   // [](pointer, std::size_t, pointer, std::size_t){}),
        deallocate_fn_(), // [](pointer, std::size_t){}),
        pptr_(nullptr),
        pn_{0}
        {}

    constexpr TWAllocator(const TWAllocator & other) :
        allocate_fn_(other.allocate_fn_),
        deallocate_fn_(other.deallocate_fn_),
        pptr_(nullptr),
        pn_(0)
        {}

    constexpr TWAllocator(TWAllocator && other) noexcept :
        allocate_fn_(std::move(other.allocate_fn_)),
        deallocate_fn_(std::move(other.deallocate_fn_)),
        pptr_(std::move(other.pptr_)),
        pn_(std::move(other.pn_))
        {
            other.pptr_ = nullptr;
            other.pn_=0;
        }

    constexpr ~TWAllocator() {
        pptr_ = nullptr;
        pn_=0;
    }

    constexpr pointer allocate(std::size_t n) {
        static_assert(sizeof(T) != 0, "cannot allocate incomplete types");
        
        pointer p = static_cast<pointer>(::operator new[](n * sizeof(T)));

        allocate_fn_(pptr_, pn_, p, n);

        pptr_=p;
        pn_=n;

        return p;
    }

    constexpr void deallocate (pointer p, std::size_t n) {
        if (!p) return;
        
        deallocate_fn_(p, n);

        ::operator delete[](p);
    }

    void SetAllocateFn(const AllocateFn & in_fn) {
        allocate_fn_ = in_fn;
    }

    void SetAllocateFn(AllocateFn && in_fn) noexcept {
        allocate_fn_ = std::move(in_fn);
    }

    void SetDeallocateFn(const DeallocateFn & in_fn) {
        deallocate_fn_ = in_fn;
    }

    void SetDeallocateFn(DeallocateFn && in_fn) noexcept {
        deallocate_fn_ = std::move(in_fn);
    }

private:

    AllocateFn allocate_fn_;
    DeallocateFn deallocate_fn_;

    pointer pptr_;
    std::size_t pn_;

};

template<typename T1,typename T2, typename AllocateFn, typename DeallocateFn>
constexpr bool operator==( const TWAllocator<T1, AllocateFn, DeallocateFn>& lhs, const TWAllocator<T2, AllocateFn, DeallocateFn>& rhs ) noexcept
{
    return std::is_same_v<T1, T2>;
}


