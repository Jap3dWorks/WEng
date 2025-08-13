#pragma once

#include "WCore/WCore.hpp"

#include <memory>
#include <stdexcept>
#include <vector>
#include <stdexcept>

template<typename T, typename Allocator=std::allocator<T>>
class WCORE_API TStack {
private:

    using DataType = std::vector<T, Allocator>;
    using Iterator = std::vector<T, Allocator>::reverse_iterator;
    using ConstIterator = std::vector<T, Allocator>::const_reverse_iterator;

public:

    constexpr TStack() noexcept=default;

    constexpr virtual ~TStack() noexcept=default;

    constexpr TStack(const TStack &)=default;

    constexpr TStack(TStack &&) noexcept=default;

    TStack & operator=(const TStack &)=default;

    TStack & operator=(TStack &&) noexcept=default;

    void Put(const T & value) {
        data_.push_back(value);
    }

    T & Top() {
        return data_.back();
    }

    const T & Top() const {
        return data_.back();
    }

     T Pop() {
         assert(data_.size() > 0);
         
         T r = data_.back();
         data_.resize(data_.size()-1, 0);
         return r;
    }

    bool Contains(const T & value) const {
        return std::find(data_.begin(), data_.end(), value) != data_.end();
    }

    constexpr Iterator begin() noexcept {
        return data_.rbegin();
    }

    constexpr Iterator end() noexcept {
        return data_.rend();
    }

    constexpr ConstIterator cbegin() const noexcept {
        return data_.crbegin();
    }

    constexpr ConstIterator cend() const noexcept {
        return data_.crend();
    }

    constexpr std::size_t Count() const noexcept {
        return data_.size();
    }

    std::size_t Position(const T & value) const {
        size_t p=0;
        for(auto & v : data_) {
            if (v == value) {
                return data_.size() -1 - p;
            }

            p++;
        }

        throw std::out_of_range("Value not present!");
    }

    T & At(const std::size_t & pos) {
        assert(pos < data_.size());
        return data_[data_.size() - 1 - pos];
    }

    const T & At(const std::size_t & pos) const {
        assert(pos < data_.size());
        return data_[data_.size() - 1 - pos];
    }

    constexpr void Clear() noexcept {
        data_.clear();
    }

private:

    DataType data_{};

};
