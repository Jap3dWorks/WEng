#pragma once

#include "WCore/WCore.h"
#include <list>
#include <cassert>

// Should be serializable?
/**
 * WId pool to generate and store unnused ids.
 */
class WCORE_API WIdPool
{

public:

    WIdPool()=default;

    WId Generate()
    {
        if (!released_.empty())
        {
            size_t ref = released_.back();
            released_.pop_back();
            return ref;
        }

        return ++last_id_;
    }

    void Release(WId wid)
    {
        assert(wid.GetId() < last_id_);

        released_.push_back(wid.GetId());
    }

private:

    size_t last_id_{0};

    std::list<size_t> released_{};
    
};

