#pragma once

#include "WCore/WCore.hpp"

#include <unordered_map>
#include <vector>
#include <cassert>

template<typename T>
class TSparseSet {
public:

    constexpr TSparseSet() noexcept = default;
    virtual ~TSparseSet() = default;

    constexpr TSparseSet(const TSparseSet& other) {
        Copy(other);
    }
    
    TSparseSet(TSparseSet && other) noexcept {
        Move(std::move(other));
    }

    constexpr TSparseSet & operator=(const TSparseSet & other) {
        Copy(other);
        return *this;
    }
    
    constexpr TSparseSet & operator=(TSparseSet && other) noexcept {
        Move(std::move(other));
        return *this;
    }
    
    void Insert(size_t in_index, const T& in_value) {
        assert(!index_pos_.contains(in_index));
        
        size_t pos = compact_.size();
        compact_.push_back(in_value);

        index_pos_.insert(in_index, pos);
        pos_index_.insert(pos,in_index);
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

    constexpr T * begin() noexcept {
        return compact_.begin();
    }

    constexpr T * end() noexcept {
        return compact_.end();
    }

    constexpr const T * cbegin() const {
        return compact_.cbegin();
    }

    constexpr const T * cend() const {
        return compact_.cend();
    }

private:

    constexpr void Copy(const TSparseSet & other) {
        pos_index_ = other.pos_index_;
        index_pos_ = other.index_pos_;
        compact_ = other.compact_;
    }

    constexpr void Move(TSparseSet && other) noexcept {
        pos_index_ = std::move(other.pos_index_);
        index_pos_ = std::move(other.index_pos_);
        compact_ = std::move(other.compact_);
    }

    std::unordered_map<size_t, size_t> pos_index_{};
    std::unordered_map<size_t, size_t> index_pos_{};
    std::vector<T> compact_{};

};
