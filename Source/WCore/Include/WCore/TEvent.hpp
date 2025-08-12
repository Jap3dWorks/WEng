#pragma once

#include "WCore/TFunction.hpp"
#include "WCore/WCore.hpp"
#include "TObjectDataBase.hpp"
#include "WCore/WConcepts.hpp"

#include <cstdint>
#include <unordered_map>
#include <utility>

template<typename ... Args>
struct TEvent {

private:
    
    using FnType = TFnLmbd16<void, Args...>;

public:

    constexpr TEvent()=default;

    virtual ~TEvent()=default;

    constexpr TEvent(const TEvent & other)=default;

    constexpr TEvent(TEvent && other)=default;

    TEvent & operator=(const TEvent & other)=default;

    TEvent & operator=(TEvent && other)=default;

    bool Emit(Args && ... args) {
        for(auto & fn : subscribers_) {
            fn(std::forward<Args>(args)...);
        }
    }

    WEventId Subscribe(const FnType & in_fn) {
        return subscribers_.Create(in_fn);
    }

    WEventId Subscribe(FnType && in_fn) {
        return subscribers_.Create(std::move(in_fn));
    }

    void Unsubscribe(const WEventId & in_id) {
        subscribers_.Remove(in_id);
    }

    void Clear() {
        subscribers_.Clear();
    }

private:

    TObjectDataBase<FnType, void, WEventId> subscribers_{};

};
