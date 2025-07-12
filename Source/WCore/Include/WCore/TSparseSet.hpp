#pragma once

#include "WCore/TIterator.hpp"
#include "WCore/WCore.hpp"

#include <memory>
#include <unordered_map>
#include <vector>
#include <cassert>

template<typename T, typename Allocator=std::allocator<T>>
class TSparseSet {
public:

    constexpr TSparseSet() noexcept = default;
    virtual ~TSparseSet() = default;

    constexpr TSparseSet(const TSparseSet& other) :
        index_pos_(other.index_pos_),
        pos_index_(other.pos_index_),
        compact_(other.compact_)
        {}
    
    TSparseSet(TSparseSet && other) noexcept :
        index_pos_(std::move(other.index_pos_)),
        pos_index_(std::move(other.pos_index_)),
        compact_(std::move(other.compact_))
        {}

    constexpr TSparseSet & operator=(const TSparseSet & other) {
        index_pos_ = other.index_pos_;
        pos_index_ = other.pos_index_;
        compact_ = other.compact_;
        
        return *this;
    }
    
    constexpr TSparseSet & operator=(TSparseSet && other) noexcept {
        index_pos_ = std::move(other.index_pos_);
        pos_index_ = std::move(other.pos_index_);
        compact_ = std::move(other.compact_);
        
        return *this;
    }
    
    void Insert(size_t in_index, const T& in_value) {
        assert(!index_pos_.contains(in_index));
        
        size_t pos = compact_.size();
        compact_.push_back(in_value);

        index_pos_[in_index] = pos;
        pos_index_[pos] = in_index;
    }

    T & Get(size_t in_index) {
        return compact_[index_pos_[in_index]];
    }

    const T & Get(size_t in_index) const {
        return compact_[index_pos_.at(in_index)];
    }

    constexpr size_t Count() const noexcept {
        return compact_.size();
    }

    void Delete(size_t in_index) {
        size_t pos = index_pos_[in_index];
        size_t last = compact_.size() - 1;

        compact_[pos] = std::move(compact_[last]);

        size_t lindex = pos_index_[last];
        index_pos_[lindex] = pos;

        index_pos_.erase(in_index);
        pos_index_.erase(last);

        compact_.resize(last);
    }

    void Clear() noexcept {
        pos_index_.clear();
        index_pos_.clear();
        compact_.clear();
    }

    constexpr void Reserve(size_t in_size) {
        compact_.reserve(in_size);
    }

    WNODISCARD bool Contains(size_t in_index) const {
        return index_pos_.contains(in_index);
    }

    constexpr std::vector<T>::iterator begin() noexcept {
        return compact_.begin();
    }

    constexpr std::vector<T>::iterator end() noexcept {
        return compact_.end();
    }

    constexpr std::vector<T>::const_iterator cbegin() const {
        return compact_.cbegin();
    }

    constexpr std::vector<T>::const_iterator cend() const {
        return compact_.cend();
    }

    constexpr auto iter_index() noexcept {
        return TIterator<size_t, decltype(index_pos_.begin())>(
            index_pos_.begin(), index_pos_.end()
            );
    }

    constexpr Allocator & GetAllocator() noexcept {
        Allocator n;
        compact_.get_allocator(n);
    }

private:

    std::unordered_map<size_t, size_t> index_pos_{};
    std::unordered_map<size_t, size_t> pos_index_{};
    std::vector<T, Allocator> compact_{};

};
