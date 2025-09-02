#pragma once

#include "WCore/TFunction.hpp"
#include "WCore/WConcepts.hpp"
#include <utility>

template<typename _ValueType,
         typename _IterType,
         typename _RetValueType,
         typename ValueFn,
         typename IncrFn
         >
    
class _TIterator {
    
public:

    using ValueType = _ValueType;
    using IterType = _IterType;
    
    using RetValueType = _RetValueType;
    // using FnValue_IterParamType=_FnValue_IterParamType;

    // using FnIncr_IterParamType=_FnIncr_IterParamType;
    // using FnIncr_RetType=_FnIncr_RetType;
    
public:

    constexpr _TIterator(const IterType & in_begin,
                        const IterType & in_end,
                        const ValueFn & in_value_fn,
                        const IncrFn & in_increse_fn
        ) noexcept :
        begin_(in_begin),
        end_(in_end),
        current_(in_begin),
        value_fn_(in_value_fn),
        increase_fn_(in_increse_fn)
        {}

    ~_TIterator() = default;

    constexpr _TIterator(const _TIterator & other) noexcept :
        begin_(other.begin_),
        end_(other.end_),
        current_(other.current_),
        value_fn_(other.value_fn_),
        increase_fn_(other.increase_fn_) {}

    constexpr _TIterator(_TIterator && other) noexcept :
        begin_(std::move(other.current_)),
        end_(std::move(other.end_)),
        current_(std::move(other.current_)),
        value_fn_(std::move(other.value_fn_)),
        increase_fn_(std::move(other.increase_fn_)) {}

    constexpr _TIterator & operator=(const _TIterator & other) noexcept {
        if (this != &other) {
            begin_ = other.begin_;
            end_ = other.end_;
            current_ = other.current_;
            value_fn_ = other.value_fn_;
            increase_fn_ = other.increase_fn_;
        }

        return *this;
    }

    constexpr _TIterator & operator=(_TIterator && other) noexcept {
        if (this != &other) {
            begin_ = std::move(other.begin_);
            end_ = std::move(other.end_);
            current_ = std::move(other.current_);
            value_fn_ = std::move(other.value_fn_);
            increase_fn_ = std::move(other.increase_fn_);
        }

        return *this;
    }

    constexpr _TIterator begin() const noexcept {
        return {begin_, end_, value_fn_, increase_fn_};
    }

    constexpr _TIterator end() const noexcept {
        return {end_, end_, value_fn_, increase_fn_};
    }

    constexpr RetValueType operator*() const {
        return value_fn_(current_, 0);
    }

    constexpr _TIterator& operator++() noexcept {
        current_ = increase_fn_(current_, 1);
        return *this;
    }

    constexpr _TIterator operator++(std::int32_t) {
        _TIterator r = *this;
        operator++();
        return r;
    }

    constexpr RetValueType operator[](std::int32_t in_value) const noexcept {
        return value_fn_(current_, in_value);
    }

    constexpr _TIterator operator+(std::int32_t in_value) const noexcept {
        return {increase_fn_(current_, in_value), end_, value_fn_, increase_fn_};
    }

    constexpr bool operator==(const _TIterator & other) const noexcept {
        return current_ == other.current_;
    }

    constexpr bool operator!=(const _TIterator & other) const noexcept {
        return current_ != other.current_;
    }

    constexpr _TIterator& operator+=(std::int32_t in_value) noexcept {
        current_ = increase_fn_(current_, in_value);
        return *this;
    }

private:

    IterType current_;
    IterType begin_;
    IterType end_;
    ValueFn value_fn_;
    IncrFn increase_fn_;
  
};

template<typename ValueType,
         typename IterType,
         typename RetValueType,
         typename ValueFn,
         typename IncrFn
         >
using TIterator = _TIterator<ValueType, IterType, RetValueType, ValueFn, IncrFn>;

template<typename T, typename ValueFn, typename IncrFn>
using TIteratorPtr = _TIterator<T, T*, T&, ValueFn, IncrFn>;

namespace WIteratorUtils {
    
    template<typename ValueType>
    inline auto DefaultIteratorPtr(ValueType * in_begin,
                                   ValueType * in_end) {
        return TIteratorPtr(in_begin,
                            in_end,
                            [](ValueType * const& _it, const std::int32_t & _i)
                            -> ValueType & {
                                return *(_it + _i);
                            },
                            [](ValueType * const& _it, const std::int32_t & _i)
                            -> ValueType * {
                                return _it + _i;
                            }
            );
    }
}





