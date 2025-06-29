#pragma once

#include "WCore/WCore.h"

#include <unordered_map>
#include <vector>
#include <cassert>

template<typename T>
class TSparseSet {
public:

    constexpr TSparseSet() noexcept = default;
    virtual ~TSparseSet() = default;

    constexpr TSparseSet(const TSparseSet& other) = default;
    constexpr TSparseSet(TSparseSet && other) noexcept = default;

    constexpr TSparseSet & operator=(const TSparseSet & other) = default;
    constexpr TSparseSet & operator=(TSparseSet && other) noexcept = default;
    

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

    WNODISCARD bool Contains(size_t in_index) const {
        return index_pos_.contains(in_index);
    }

    T * begin() {
        return compact_.begin();
    }

    T * end() {
        return compact_.end();
    }

    constexpr const T * cbegin() const {
        return compact_.cbegin();
    }

    constexpr const T * cend() const {
        return compact_.cend();
    }

private:

    std::unordered_map<size_t, size_t> pos_index_{};
    std::unordered_map<size_t, size_t> index_pos_{};
    std::vector<T> compact_{};

};
