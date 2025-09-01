#pragma once

#include "WCore/TFunction.hpp"
#include <utility>

template<typename _ValueType,
         typename _IterType,
         typename _ValueRetType=_ValueType&,
         typename _FnValue_IterParamType=const _IterType&,
         typename _FnIncr_IterParamType=_IterType&,
         typename _FnIncr_RetType=_IterType>
class TIterator {
    
public:

    using ValueType = _ValueType;
    using IterType = _IterType;
    
    using RetValueType = _ValueRetType;
    using FnValue_IterParamType=_FnValue_IterParamType;

    using FnIncr_IterParamType=_FnIncr_IterParamType;
    using FnIncr_RetType=_FnIncr_RetType;
    
    using ValueFn = TFnLmbd32<RetValueType, FnValue_IterParamType, const std::int32_t &>;
    using IncrFn = TFnLmbd32<FnIncr_RetType, FnIncr_IterParamType, const std::int32_t &>;

public:

    constexpr TIterator(const IterType & in_begin,
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

    ~TIterator() = default;

    constexpr TIterator(const TIterator & other) noexcept :
        begin_(other.begin_),
        end_(other.end_),
        current_(other.current_),
        value_fn_(other.value_fn_),
        increase_fn_(other.increase_fn_) {}

    constexpr TIterator(TIterator && other) noexcept :
        begin_(std::move(other.current_)),
        end_(std::move(other.end_)),
        current_(std::move(other.current_)),
        value_fn_(std::move(other.value_fn_)),
        increase_fn_(std::move(other.increase_fn_)) {}

    constexpr TIterator & operator=(const TIterator & other) noexcept {
        if (this != &other) {
            begin_ = other.begin_;
            end_ = other.end_;
            current_ = other.current_;
            value_fn_ = other.value_fn_;
            increase_fn_ = other.increase_fn_;
        }

        return *this;
    }

    constexpr TIterator & operator=(TIterator && other) noexcept {
        if (this != &other) {
            begin_ = std::move(other.begin_);
            end_ = std::move(other.end_);
            current_ = std::move(other.current_);
            value_fn_ = std::move(other.value_fn_);
            increase_fn_ = std::move(other.increase_fn_);
        }

        return *this;
    }

    constexpr TIterator begin() const noexcept {
        return {begin_, end_, value_fn_, increase_fn_};
    }

    constexpr TIterator end() const noexcept {
        return {end_, end_, value_fn_, increase_fn_};
    }

    constexpr RetValueType operator*() const {
        return value_fn_(current_, 0);
    }

    constexpr TIterator& operator++() noexcept {
        current_ = increase_fn_(current_, 1);
        return *this;
    }

    constexpr TIterator operator++(std::int32_t) {
        TIterator r = *this;
        operator++();
        return r;
    }

    constexpr RetValueType operator[](std::int32_t in_value) const noexcept {
        return value_fn_(current_, in_value);
    }

    constexpr TIterator operator+(std::int32_t in_value) const noexcept {
        return {increase_fn_(current_, in_value), end_, value_fn_, increase_fn_};
    }

    constexpr bool operator==(const TIterator & other) const noexcept {
        return current_ == other.current_;
    }

    constexpr bool operator!=(const TIterator & other) const noexcept {
        return current_ != other.current_;
    }

    constexpr TIterator& operator+=(std::int32_t in_value) noexcept {
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

template<typename T>
using TIteratorPtr = TIterator<T, T*, T&>;


namespace WIteratorUtils {
    
    template<typename T>
    inline TIteratorPtr<T> DefaultIteratorPtr(const typename TIteratorPtr<T>::IterType & in_begin,
                                              const typename TIteratorPtr<T>::IterType & in_end) {
        return TIteratorPtr<T>(in_begin,
                               in_end,
                               [](TIteratorPtr<T>::FnValue_IterParamType _it, const std::int32_t & _i)
                               -> TIteratorPtr<T>::RetValueType {
                                   return *(_it + _i);
                               },
                               [](TIteratorPtr<T>::FnIncr_IterParamType _it, const std::int32_t & _i)
                               -> TIteratorPtr<T>::FnIncr_RetType {
                                   return _it + _i;
                               }
            );
    }
}
