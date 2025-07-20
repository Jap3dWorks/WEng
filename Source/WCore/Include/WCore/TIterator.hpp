#pragma once

#include "WCore/TFunction.hpp"
#include <utility>

template<typename _ValueType,
         typename _IterType,
         typename _ValueRetType=_ValueType&,
         typename _ConstValueRetType=const _ValueType&>
class TIterator {
    
public:

    using ValueType = _ValueType;
    using IterType = _IterType;
    using ConstValueRetType = _ConstValueRetType;
    using ValueRetType = _ValueRetType;
    using ValueFn = TFunction<ValueRetType(IterType&, const size_t &)>;

public:

    constexpr TIterator(const IterType & in_begin,
                        const IterType & in_end) noexcept :
        begin_(in_begin),
        end_(in_end),
        current_(in_begin),
        value_fn_(
            [](IterType & _iter, const size_t & _idx) -> ValueRetType
            { return * (_iter + _idx); })
        {}

    constexpr TIterator(const IterType & in_begin,
                        const IterType & in_end,
                        const ValueFn & in_value_fn) noexcept :
        begin_(in_begin),
        end_(in_end),
        current_(in_begin),
        value_fn_(in_value_fn) {}

    ~TIterator() = default;

    constexpr TIterator(const TIterator & other) noexcept :
        begin_(other.begin_),
        end_(other.end_),
        current_(other.current_),
        value_fn_(other.value_fn_) {}

    constexpr TIterator(TIterator && other) noexcept :
        begin_(std::move(other.current_)),
        end_(std::move(other.end_)),
        current_(std::move(other.current_)),
        value_fn_(std::move(other.value_fn_)) {}

    constexpr TIterator & operator=(const TIterator & other) noexcept {
        if (this != &other) {
            begin_=other.begin_;
            end_=other.end_;
            current_=other.current_;
            value_fn_=other.value_fn_;
        }

        return *this;
    }

    constexpr TIterator & operator=(TIterator && other) noexcept {
        if (this != &other) {
            begin_=std::move(other.begin_);
            end_=std::move(other.end_);
            current_=std::move(other.current_);
            value_fn_=std::move(other.value_fn_);
        }

        return *this;
    }

    constexpr TIterator begin() const noexcept {
        return {begin_, end_, value_fn_};
    }

    constexpr TIterator end() const noexcept {
        return {end_, end_, value_fn_};
    }

    constexpr ValueRetType operator*() {
        return value_fn_(current_, 0);
    }
    
    constexpr ConstValueRetType operator*() const {
        return value_fn_(current_, 0);
    }

    constexpr TIterator& operator++() noexcept {
        current_++;
        return *this;
    }

    constexpr TIterator operator++(int) {
        TIterator r = *this;
        operator++();
        return r;
    }

   constexpr ValueRetType operator[](int in_value) const noexcept {
       return value_fn_(current_, in_value);
   }

    constexpr TIterator operator+(int in_value) const noexcept {
        return {current_ + in_value, end_, value_fn_};
    }

    constexpr TIterator operator-(int in_value) const noexcept {
        return {current_ - in_value, end_, value_fn_};
    }

    constexpr bool operator==(const TIterator & other) noexcept {
        return current_ == other.current_;
    }

    constexpr bool operator!=(const TIterator & other) noexcept {
        return current_ != other.current_;
    }

    constexpr TIterator& operator+=(int in_value) noexcept {
        current_ += in_value;
        return *this;
    }

    constexpr TIterator& operator-=(int in_value) noexcept {
        current_ -= in_value;
        return *this;
    }

private:

    IterType current_;
    IterType begin_;
    IterType end_;
    ValueFn value_fn_;
  
};

template<typename T>
using TIteratorPtr = TIterator<T, T*, T&, const T&>;


