#pragma once

#include "WCore/TFunction.hpp"
#include <utility>

template<typename ValueType, typename IterType>
class TIterator {
public:

    using ValueFn = TFunction<ValueType&(IterType&)>;

    constexpr TIterator(IterType in_begin, IterType in_end) :
        begin_(in_begin),
        end_(in_end),
        current_(in_begin),
        value_fn_([](IterType& _iter) -> ValueType& { return *_iter; })
        {}

    constexpr TIterator(IterType in_begin, IterType in_end, const ValueFn & in_value_fn) :
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

    constexpr ValueType& operator*() {
        return value_fn_(current_);
    }
    
    constexpr const ValueType& operator*() const {
        return value_fn_(current_);
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

//    constexpr ValueType& operator[](int in_value) const noexcept {
//        return current_[in_value];
//    }

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

    IterType current_{nullptr};
    IterType begin_{nullptr};
    IterType end_{nullptr};
    ValueFn value_fn_;
  
};

template<typename T>
using TIteratorPtr = TIterator<T, T*>;


