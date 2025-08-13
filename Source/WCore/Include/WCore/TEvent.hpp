#pragma once

#include "WCore/TFunction.hpp"
#include "WCore/WCore.hpp"
#include "WCore/TSparseSet.hpp"
#include "WCore/WIdPool.hpp"

#include <cstdint>
#include <unordered_map>
#include <utility>

template<typename ... Args>
struct TEvent {

public:
    
    using FnType = TFnLmbd16<void, Args...>;

public:

    constexpr TEvent()=default;

    virtual ~TEvent()=default;

    constexpr TEvent(const TEvent & other)=default;

    constexpr TEvent(TEvent && other)=default;

    TEvent & operator=(const TEvent & other)=default;

    TEvent & operator=(TEvent && other)=default;

    bool Emit(Args ... args) {
        for(auto & fn : subscribers_) {
            fn(args...);
        }
    }

    WEventId Subscribe(const FnType & in_fn) {
        WEventId id = id_pool_.Generate();
        subscribers_.Insert(id.GetId(), in_fn);
        return id;
    }

    WEventId Subscribe(FnType && in_fn) {
        WEventId id = id_pool_.Generate();
        subscribers_.Insert(id.GetId(), std::move(in_fn));
        return id;
    }

    void Unsubscribe(const WEventId & in_id) {
        subscribers_.Remove(in_id.GetId());
    }

    void Clear() {
        subscribers_.Clear();
    }

private:

    // TODO SparseSet and WIDPool;

    TSparseSet<FnType> subscribers_{};

    WIdPool<WEventId> id_pool_{};

    // TObjectDataBase<FnType, FnType, WEventId> subscribers_{};

};
