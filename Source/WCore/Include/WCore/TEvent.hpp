#pragma once

// #include "WCore/TFunction.hpp"
#include "WCore/WCore.hpp"
#include "WCore/TSparseSet.hpp"
#include "WCore/IdPool.hpp"

#include <cstdint>
#include <unordered_map>
#include <utility>
#include <functional>

template<typename T>
struct TEvent;

template<typename RetType, typename ... Args>
struct TEvent<RetType(Args...)> {

public:

    using FnType = std::function<RetType(Args...)>;

public:

    constexpr TEvent()=default;

    virtual ~TEvent()=default;

    constexpr TEvent(const TEvent & other)=default;

    constexpr TEvent(TEvent && other)=default;

    TEvent & operator=(const TEvent & other)=default;

    TEvent & operator=(TEvent && other)=default;

    // TODO FArgs

    template<typename ... FArgs>
    bool Emit(FArgs && ... args) {
        for(const auto & fn : subscribers_) {
            fn(std::forward<FArgs>(args)...);
        }
        
        return true;
    }

    wcr::wid::WEventId Subscribe(const FnType & in_fn) {
        wcr::wid::WEventId id = id_pool_.Generate();
        subscribers_.Insert(id.GetId(), in_fn);
        return id;
    }

    wcr::wid::WEventId Subscribe(FnType && in_fn) {
        wcr::wid::WEventId id = id_pool_.Generate();
        subscribers_.Insert(id.GetId(), std::move(in_fn));
        return id;
    }

    void Unsubscribe(const wcr::wid::WEventId & in_id) {
        subscribers_.Remove(in_id.GetId());
    }

    void Clear() {
        subscribers_.Clear();
    }

private:

    TSparseSet<FnType> subscribers_{};

    wcr::IdPool<wcr::wid::WEventId::IdType> id_pool_{};

};
