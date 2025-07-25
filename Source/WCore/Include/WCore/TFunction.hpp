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
        virtual Ret Invoke(Args... args) const = 0;
        virtual std::unique_ptr<CallableBase> Clone()=0;
    };

    // Concrete wrapper for callable objects
    template<typename F>
    struct Callable : CallableBase {
        using f_type = F;

        F func;

        Callable(F&& f) : func(std::forward<F>(f)) {}

        virtual ~Callable()=default;

        Ret Invoke(Args ... args) const override {
            return func(std::forward<Args>(args)...);
        }

        constexpr Callable(const Callable & other) noexcept
            : func(other.func) {}

        constexpr Callable(Callable && other) noexcept
            : func(std::move(other.func)) {}

        constexpr Callable & operator=(const Callable & other) {
            func = other.func;
            return *this;
        }

        const Callable & operator=(Callable && other) noexcept {
            func = std::move(other.func);
            return *this;
        }

        std::unique_ptr<CallableBase> Clone() override {
            return std::make_unique<Callable<F>>(*this);
        }

    };

    std::unique_ptr<CallableBase> callable;

public:

    // Constructor from function pointer
    TFunction(Ret(*f)(Args...)) : callable(std::make_unique<Callable<decltype(f)>>(f)) {}

    TFunction(const TFunction & other) :
        callable(other.callable->Clone())
    {}

    TFunction(TFunction & other) :
        callable(other.callable->Clone())
        {}

    constexpr TFunction(TFunction && other) noexcept :
        callable(std::move(other.callable)) {
        other.callable = nullptr;
    }

    // Constructor from any callable (lambda, functor, etc.)
    template<typename F>
    TFunction(F&& f) :
    callable(std::make_unique<Callable<std::decay_t<F>>>(std::forward<F>(f))) {}

    TFunction & operator=(const TFunction & other) {
        if (this != &other) {
            callable = other.callable->Clone();
        }
        return *this;
    }

    TFunction & operator=(TFunction && other) {
        if (this != &other) {
            callable = std::move(other.callable);            
            other.callable = nullptr;
        }
        return *this;
    }

    Ret operator()(Args... args) const {
        if (!callable) {
            throw std::bad_function_call();
        }
        return callable->Invoke(std::forward<Args>(args)...);
    }

    /**
     * Empty state check
     */
    explicit operator bool() const noexcept {
        return static_cast<bool>(callable);
    }
};
