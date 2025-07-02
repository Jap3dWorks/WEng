#pragma once

#include <utility>

template<typename T>
class TIterator {
public:

    constexpr TIterator(T* in_begin, T* in_end) : begin_(in_begin), end_(in_end), ptr_(in_begin) {}

    ~TIterator() = default;

    TIterator(const TIterator & other) noexcept {
        Copy(other);
    }

    TIterator(TIterator && other) noexcept {
        Move(other);
    }

    constexpr T* begin() const noexcept {return begin;}
    constexpr T* end() const noexcept {return end;}

    constexpr const T* cbegin() const noexcept {return begin;}
    constexpr const T* cend() const noexcept {return end;}

    constexpr T& operator*() {return *ptr_;}
    constexpr const T& operator*() const {return *ptr_; }

    constexpr TIterator& operator++() {ptr_++; return *this;}
    // constexpr TIterator& operator++() {}


private:

    constexpr void Copy(const TIterator & other) noexcept {
        ptr_ = other.ptr_;
        begin_ = other.begin_;
        end_ = other.end_;        
    }

    void Move(TIterator && other) noexcept {
        ptr_ = std::move(other.ptr_);
        begin_ = std::move(other.begin_);
        end_ = std::move(other.end_);
    }

    T* ptr_{nullptr};
    T* begin_{nullptr};
    T* end_{nullptr};
  
};
