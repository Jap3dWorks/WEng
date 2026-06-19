#pragma once

#include "WCore/WConcepts.hpp"
#include "WCore/WCore.hpp"
#include "WCore/TIterator.hpp"

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
        index_stack_(),
        compact_(in_allocator)
        {}

    constexpr TSparseSet(const TSparseSet & other) :
        index_pos_(other.index_pos_),
        index_stack_(other.index_stack_),
        compact_(other.compact_)
        {}
    
    TSparseSet(TSparseSet && other) noexcept :
        index_pos_(std::move(other.index_pos_)),
        index_stack_(std::move(other.index_stack_)),
        compact_(std::move(other.compact_))
        {}

    constexpr TSparseSet & operator=(const TSparseSet & other) {
        if (this != &other) {
            index_pos_ = other.index_pos_;
            index_stack_ = other.index_stack_;
            compact_ = other.compact_;
        }

        return *this;
    }
    
    constexpr TSparseSet & operator=(TSparseSet && other) noexcept {
        if (this != &other) {
            index_pos_ = std::move(other.index_pos_);
            index_stack_ = std::move(other.index_stack_);
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

            index_stack_.push_back(in_index);
        }
    }

    T & Get(size_t in_index) {
        assert(index_pos_.contains(in_index));
        return compact_[index_pos_[in_index]];
    }

    const T & Get(size_t in_index) const {
        assert(index_pos_.contains(in_index));
        return compact_[index_pos_.at(in_index)];
    }

    std::size_t IndexInPos(std::size_t in_pos) const {
        return index_stack_[in_pos];
    }

    constexpr size_t Count() const noexcept {
        return compact_.size();
    }

    void Remove(size_t in_index) {
        size_t pos = index_pos_[in_index];
        size_t last_index = index_stack_.back();
        
        compact_[pos] = std::move(compact_.back());

        index_pos_[last_index] = pos;

        index_pos_.erase(in_index);

        compact_.pop_back();
        index_stack_.pop_back();
    }

    void Clear() noexcept {
        index_pos_.clear();
        compact_.clear();
        index_stack_.clear();
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

    template<CCallable<void, std::size_t, T&> TFn>
    void ForEach(TFn && in_predicate) {
        std::size_t i=0;
        for(auto & v: *this) {
            std::forward<TFn>(in_predicate)(index_stack_[i++], v);
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
        return index_stack_[in_pos];
    }

private:

    std::unordered_map<size_t, size_t> index_pos_;

    std::vector<size_t > index_stack_;

    std::vector<T, Allocator> compact_;

};


