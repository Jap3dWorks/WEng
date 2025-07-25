#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TIterator.hpp"

#include <memory>
#include <unordered_map>
#include <vector>
#include <cassert>

template<typename T, typename Allocator=std::allocator<T>>
class TSparseSet {

public:

    using IndexPosType = std::unordered_map<size_t, size_t>;

    using IndexIterator = TIterator<size_t,
                                    IndexPosType::iterator,
                                    const size_t &
                                    >;

    using ConstIndexIterator = TIterator<size_t,
                                         IndexPosType::const_iterator,
                                         const size_t &>;

public:

    constexpr TSparseSet() noexcept = default;

    virtual ~TSparseSet() = default;

    constexpr TSparseSet(const Allocator & in_allocator) :
        index_pos_(),
        pos_index_(),
        compact_(in_allocator)
        {}

    constexpr TSparseSet(const TSparseSet & other) :
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

    constexpr std::vector<T, Allocator>::iterator begin() noexcept {
        return compact_.begin();
    }

    constexpr std::vector<T, Allocator>::iterator end() noexcept {
        return compact_.end();
    }

    constexpr std::vector<T, Allocator>::const_iterator cbegin() const {
        return compact_.cbegin();
    }

    constexpr std::vector<T, Allocator>::const_iterator cend() const {
        return compact_.cend();
    }

    IndexIterator IterIndexes() {
        return IndexIterator(
            index_pos_.begin(),
            index_pos_.end(),
            [](IndexIterator::IterType & _iter, const size_t & _idx) -> const size_t & {
                return (*_iter).first;
            }
            );
    }

    ConstIndexIterator IterIndexes() const {
        return ConstIndexIterator(
            index_pos_.cbegin(),
            index_pos_.cend(),
            [] (ConstIndexIterator::IterType & _iter, const size_t & _idx) -> const size_t & {
                return (*_iter).first;
            }
            );
    }

private:

    std::unordered_map<size_t, size_t> index_pos_;

    std::unordered_map<size_t, size_t> pos_index_;

    std::vector<T, Allocator> compact_;

};
