#pragma once

#include "WCore/WConcepts.hpp"
#include "WLog.hpp"

#include <cstring>
#include <functional>
#include <memory>
#include <concepts>
#include <type_traits>

template<typename T>
struct TFnPtr;

/**
 * @brief Wrapper to a function pointer.
 */
template<typename Ret, typename ...Args>
struct TFnPtr<Ret(Args...)> {

    constexpr TFnPtr() = default;

    constexpr virtual ~TFnPtr() = default;

    constexpr TFnPtr(Ret(*in_fn)(Args...)) : fn_(in_fn) {}

    constexpr TFnPtr(const TFnPtr & other) = default;

    constexpr TFnPtr(TFnPtr && other) = default;

    constexpr TFnPtr & operator=(const TFnPtr & other) =default;

    constexpr TFnPtr & operator=(TFnPtr && other) =default;

    template<typename ... FArgs>
    Ret operator()(FArgs && ... args) const {
        if (!fn_) {
            throw std::bad_function_call();
        }
        
        return fn_(std::forward<FArgs>(args)...);
    }

    bool IsValid() const {
        return static_cast<bool>(fn_);
    }

    bool IsEmpty() const {
        return !IsValid();
    }

private:
    
    Ret(*fn_)(Args...);

};
