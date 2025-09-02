#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TIterator.hpp"
#include "TFunction.hpp"

#include <concepts>
#include <memory>
#include <unordered_map>
#include <vector>
#include <cassert>

template<typename T, typename Allocator=std::allocator<T>>
class TSparseSet {

public:

    using IndexPosType = std::unordered_map<size_t, size_t>;

    template<typename ValueFn, typename IncrFn>
    using ConstIndexIterator = TIterator<size_t,
                                         IndexPosType::const_iterator,
                                         const size_t &,
                                         ValueFn,
                                         IncrFn>;

    using Iterator = std::vector<T, Allocator>::iterator;

    using ConstIterator = std::vector<T,Allocator>::const_iterator;

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
        if (this != &other) {
            index_pos_ = other.index_pos_;
            pos_index_ = other.pos_index_;
            compact_ = other.compact_;
        }

        return *this;
    }
    
    constexpr TSparseSet & operator=(TSparseSet && other) noexcept {
        if (this != &other) {
            index_pos_ = std::move(other.index_pos_);
            pos_index_ = std::move(other.pos_index_);
            compact_ = std::move(other.compact_);
        }

        return *this;
    }

    template<std::convertible_to<T> D>
    void Insert(const size_t & in_index, D && in_value) {
        if (Contains(in_index)) {
            size_t pos = index_pos_[in_index];
            compact_[pos]=std::forward<D>(in_value);
        }
        else {
            size_t pos = compact_.size();
            compact_.push_back(std::forward<D>(in_value));

            index_pos_[in_index] = pos;
            pos_index_[pos] = in_index;            
        }
    }

    // void Insert(const size_t & in_index, const T& in_value) {
    //     if (Contains(in_index)) {
    //         size_t pos = index_pos_[in_index];
    //         compact_[pos]=in_value;
    //     }
    //     else {
    //         size_t pos = compact_.size();
    //         compact_.push_back(in_value);

    //         index_pos_[in_index] = pos;
    //         pos_index_[pos] = in_index;            
    //     }
    // }

    // void Insert(const size_t & in_index, T && in_value) {
    //     if (Contains(in_index)) {
    //         size_t pos = index_pos_[in_index];
    //         compact_[pos]=in_value;
    //     }
    //     else {
    //         size_t pos = compact_.size();
    //         compact_.push_back(std::move(in_value));

    //         index_pos_[in_index] = pos;
    //         pos_index_[pos] = in_index;            
    //     }

    // }

    T & Get(size_t in_index) {
        assert(index_pos_.contains(in_index));
        return compact_[index_pos_[in_index]];
    }

    const T & Get(size_t in_index) const {
        assert(index_pos_.contains(in_index));
        return compact_[index_pos_.at(in_index)];
    }

    std::size_t IndexInPos(std::size_t in_pos) const {
        return pos_index_.at(in_pos);
    }

    constexpr size_t Count() const noexcept {
        return compact_.size();
    }

    void Remove(size_t in_index) {
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

    constexpr Iterator begin() noexcept {
        return compact_.begin();
    }

    constexpr Iterator end() noexcept {
        return compact_.end();
    }

    constexpr ConstIterator begin() const noexcept {
        return compact_.begin();
    }

    constexpr ConstIterator end() const noexcept {
        return compact_.end();
    }

    constexpr ConstIterator cbegin() const noexcept {
        return compact_.cbegin();
    }

    constexpr ConstIterator cend() const noexcept {
        return compact_.cend();
    }

    void ForEach(TFunction<void(std::size_t, T&)> in_predicate) {
        std::size_t i=0;
        for(auto & v: *this) {
            in_predicate(pos_index_.at(i++), v);
        }
    }

    auto IterIndexes() const {
        return ConstIndexIterator(
            index_pos_.cbegin(),
            index_pos_.cend(),
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
        return pos_index_.at(in_pos);
    }

private:

    std::unordered_map<size_t, size_t> index_pos_;

    std::unordered_map<size_t, size_t> pos_index_;

    std::vector<T, Allocator> compact_;

};


