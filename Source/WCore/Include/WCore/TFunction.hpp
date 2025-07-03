#pragma once

#include <functional>
#include <memory>

template<typename T>
struct TFunction;

// Partial specialization for function types
template<typename Ret, typename... Args>
struct TFunction<Ret(Args...)> {
private:
    // Type erasure base class
    struct CallableBase {
        virtual ~CallableBase() = default;
        virtual Ret invoke(Args... args) = 0;
    };

    // Concrete wrapper for callable objects
    template<typename F>
    struct Callable : CallableBase {
        F func;
        Callable(F&& f) : func(std::forward<F>(f)) {}
        Ret invoke(Args... args) override {
            return func(std::forward<Args>(args)...);
        }
    };

    std::unique_ptr<CallableBase> callable;

public:

    // Constructor from function pointer
    TFunction(Ret(*f)(Args...)) : callable(std::make_unique<Callable<decltype(f)>>(f)) {}

    // Constructor from any callable (lambda, functor, etc.)
    template<typename F>
    TFunction(F&& f) :
    callable(std::make_unique<Callable<std::decay_t<F>>>(std::forward<F>(f))) {}

    Ret operator()(Args... args) {
        if (!callable) {
            throw std::bad_function_call();
        }
        return callable->invoke(std::forward<Args>(args)...);
    }

    // Empty state check
    explicit operator bool() const noexcept {
        return static_cast<bool>(callable);
    }
};
