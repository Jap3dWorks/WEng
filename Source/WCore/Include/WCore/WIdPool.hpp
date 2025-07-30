#pragma once

#include "WCore/WCore.hpp"
#include <list>
#include <cassert>
#include <unordered_set>

// TODO Should be serializable?
/**
 * WId pool to generate and store unnused ids.
 */
template<typename T=WId>
class WCORE_API WIdPool
{

public:

    constexpr WIdPool() noexcept =default;

    virtual ~WIdPool() noexcept {
        Clear();
    }

    WIdPool(const WIdPool & other) :
    last_id_(other.last_id_),
    reserved_(other.reserved_),
    released_(other.released_) {}

    WIdPool(WIdPool && other) :
    last_id_(std::move(other.last_id_)),
    reserved_(std::move(other.reserved_)),
    released_(std::move(other.released_)) {}

    WIdPool & operator=(const WIdPool & other) {
        if (this != &other) {
            last_id_ = other.last_id_;
            reserved_ = other.reserved_;
            released_ = other.released_;
        }

        return *this;
    }

    WIdPool & operator=(WIdPool && other) {
        if (this != &other) {
            last_id_ = std::move(other.last_id_);
            reserved_ = std::move(other.reserved_);
            released_ = std::move(other.released_);
        }

        return *this;
    }

    T Generate()
    {
        T r = 0;
        if (!released_.empty())
        {
            r = released_.back();
            released_.pop_back();
        }
        else {
            r = ++last_id_;
            while(reserved_.contains(r)) {
                reserved_.erase(r);
                r = ++last_id_;
            }
        }

        return r;
    }

    void Release(T wid)
    {
        assert(wid.GetId() < last_id_.GetId() || reserved_.contains(wid));

        if (reserved_.contains(wid)) {
            reserved_.erase(wid);
        }
        
        released_.push_back(wid);
    }

    void Reserve(T in_id) {
        assert(!reserved_.contains(in_id));
        
        released_.remove(in_id);
        if (in_id >= last_id_) {
            reserved_.insert(in_id);
        }
    }

    void Clear() noexcept {
        last_id_ = 0;
        released_.clear();
        reserved_.clear();
    }

private:

    T last_id_{0};
    std::unordered_set<T> reserved_{};
    std::list<T> released_{};

};

