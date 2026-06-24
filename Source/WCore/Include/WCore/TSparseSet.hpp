#pragma once

#include "WCore/WConcepts.hpp"
#include "WCore/WCore.hpp"
#include "WCore/TIterator.hpp"

#include <concepts>
#include <memory>
#include <unordered_map>
#include <vector>
#include <cassert>

template<typename T,
         typename ValueAllocator=std::allocator<T>,
         typename IndexAllocator=std::allocator<std::size_t>>
class TSparseSet {

public:

    using IndexPosType = std::unordered_map<size_t, size_t>;

    template<typename ValueFn, typename IncrFn>
    using ConstIndexIterator = TIterator<size_t,
                                         IndexPosType::const_iterator,
                                         const size_t &,
                                         ValueFn,
                                         IncrFn>;

    using Iterator = std::vector<T, ValueAllocator>::iterator;
    using ConstIterator = std::vector<T,ValueAllocator>::const_iterator;

public:

    constexpr TSparseSet() noexcept = default;

    virtual ~TSparseSet() = default;

    constexpr TSparseSet(const ValueAllocator & in_value_allocator) :
        index_pos_map_(),
        index_dense_(),
        value_dense_(in_value_allocator)
        {}

    constexpr TSparseSet(const ValueAllocator & in_value_allocator,
                         const IndexAllocator & in_index_allocator) :
        index_pos_map_(),
        index_dense_(in_index_allocator),
        value_dense_(in_value_allocator)
        {}

    constexpr TSparseSet(const TSparseSet & other) :
        index_pos_map_(other.index_pos_map_),
        index_dense_(other.index_dense_),
        value_dense_(other.value_dense_)
        {}
    
    TSparseSet(TSparseSet && other) noexcept :
        index_pos_map_(std::move(other.index_pos_map_)),
        index_dense_(std::move(other.index_dense_)),
        value_dense_(std::move(other.value_dense_))
        {}

    constexpr TSparseSet & operator=(const TSparseSet & other) {
        if (this != &other) {
            index_pos_map_ = other.index_pos_map_;
            index_dense_ = other.index_dense_;
            value_dense_ = other.value_dense_;
        }

        return *this;
    }
    
    constexpr TSparseSet & operator=(TSparseSet && other) noexcept {
        if (this != &other) {
            index_pos_map_ = std::move(other.index_pos_map_);
            index_dense_ = std::move(other.index_dense_);
            value_dense_ = std::move(other.value_dense_);
        }

        return *this;
    }

    template<std::convertible_to<T> D>
    void Insert(const size_t & in_index, D && in_value) {
        if (Contains(in_index)) {
            size_t pos = index_pos_map_[in_index];
            value_dense_[pos]=std::forward<D>(in_value);
        }
        else {
            size_t pos = value_dense_.size();
            value_dense_.push_back(std::forward<D>(in_value));

            index_pos_map_[in_index] = pos;

            index_dense_.push_back(in_index);
        }
    }

    T & Get(size_t in_index) {
        return value_dense_[index_pos_map_[in_index]];
    }

    const T & Get(size_t in_index) const {
        return value_dense_[index_pos_map_.at(in_index)];
    }

    std::size_t DensePosition(std::size_t in_pos) const {
        return index_dense_[in_pos];
    }

    constexpr size_t Count() const noexcept {
        return value_dense_.size();
    }

    void Remove(size_t in_index) {
        size_t pos = index_pos_map_[in_index];
        size_t last_index = index_dense_.back();
        
        value_dense_[pos] = std::move(value_dense_.back());

        index_pos_map_[last_index] = pos;

        index_pos_map_.erase(in_index);

        value_dense_.pop_back();
        index_dense_.pop_back();
    }

    void Clear() noexcept {
        index_pos_map_.clear();
        value_dense_.clear();
        index_dense_.clear();
    }

    constexpr void Reserve(size_t in_size) {
        value_dense_.reserve(in_size);
    }

    WNODISCARD bool Contains(size_t in_index) const {
        return index_pos_map_.contains(in_index);
    }

    constexpr Iterator begin() noexcept {
        return value_dense_.begin();
    }

    constexpr Iterator end() noexcept {
        return value_dense_.end();
    }

    constexpr ConstIterator begin() const noexcept {
        return value_dense_.begin();
    }

    constexpr ConstIterator end() const noexcept {
        return value_dense_.end();
    }

    constexpr ConstIterator cbegin() const noexcept {
        return value_dense_.cbegin();
    }

    constexpr ConstIterator cend() const noexcept {
        return value_dense_.cend();
    }

    template<CCallable<void, std::size_t, T&> TFn>
    void ForEach(TFn && in_predicate) {
        std::size_t i=0;
        for(auto & v: *this) {
            std::forward<TFn>(in_predicate)(index_dense_[i++], v);
        }
    }

    auto IterIndexes() const {
        return ConstIndexIterator(
            index_pos_map_.cbegin(),
            index_pos_map_.cend(),
            [] (auto & _it, const std::int32_t & _i) -> const size_t & {
                return (*_it).first;
            },
            [](auto & _it, const std::int32_t & _i) -> IndexPosType::const_iterator {
                _it++;
                return _it;
            }
            );
    }

    std::size_t IndexAt(std::size_t in_pos) const {
        return index_dense_[in_pos];
    }

private:

    std::unordered_map<std::size_t, std::size_t> index_pos_map_;
    std::vector<std::size_t, IndexAllocator> index_dense_;
    std::vector<T, ValueAllocator> value_dense_;

};


