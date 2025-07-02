#pragma once

#include <utility>

template<typename ValueType, typename IterType>
class TIterator {
public:

    constexpr TIterator(IterType in_begin, IterType in_end) :
        begin_(in_begin), end_(in_end), current_(in_begin) {}

    ~TIterator() = default;

    constexpr TIterator(const TIterator & other) noexcept {
        Copy(other);
    }

    constexpr TIterator(TIterator && other) noexcept {
        Move(other);
    }

    constexpr TIterator & operator=(const TIterator & other) noexcept {
        Copy(other);
        return *this;
    }

    constexpr TIterator & operator=(TIterator && other) noexcept {
        Move(other);
        return *this;
    }

    constexpr TIterator begin() const noexcept {return {begin_, end_}; }
    constexpr TIterator end() const noexcept {return {end_, end_}; }

    constexpr ValueType& operator*() {return *current_;}
    constexpr const ValueType& operator*() const {return *current_; }

    constexpr TIterator& operator++() noexcept {current_++; return *this;}
    constexpr TIterator operator++(int) {
        TIterator r = *this;
        operator++();
        return r;
    }

    constexpr ValueType& operator[](int in_value) const noexcept {
        return current_[in_value];
    }

    constexpr TIterator operator+(int in_value) const noexcept {
        return {current_ + in_value, end_};
    }

    constexpr TIterator operator-(int in_value) const noexcept {
        return {current_ - in_value, end_};
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

    constexpr void Copy(const TIterator & other) noexcept {
        current_ = other.current_;
        begin_ = other.begin_;
        end_ = other.end_;        
    }

    constexpr void Move(TIterator && other) noexcept {
        current_ = std::move(other.current_);
        begin_ = std::move(other.begin_);
        end_ = std::move(other.end_);
    }

    IterType current_{nullptr};
    IterType begin_{nullptr};
    IterType end_{nullptr};
  
};

template<typename T>
using TIteratorPtr = TIterator<T, T*>;


