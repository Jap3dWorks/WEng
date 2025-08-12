#pragma once

#include "WCore/WConcepts.hpp"

#include <cstring>
#include <functional>
#include <memory>
#include <concepts>
#include <type_traits>

template<typename T>
struct TFunction;

/**
 * @brief General use function wrapper. Contains a virtual call.
 */
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

        Callable(const F& f) : func(f) {}

        Callable(F && f ) : func(std::move(f)) {}

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
        using f_type = F;

        F func;

    };

public:

    constexpr TFunction()=default;

    constexpr virtual ~TFunction()=default;

    TFunction(const TFunction & other) :
        callable_(other.callable_->Clone())
    {}

    constexpr TFunction(TFunction && other) noexcept :
        callable_(std::move(other.callable_)) {
        other.callable_ = nullptr;
    }

    /** @brief Constructor from function pointer */
    TFunction(Ret(*f)(Args...)) :
        callable_(std::make_unique<Callable<Ret(*)(Args...)>>(f))
        {}

    /** @brief Constructor from any callable (lambda, functor, etc.) */
    template<typename F>
    TFunction(F && f) :
    callable_(std::make_unique< Callable<std::decay_t<F>> >(std::forward<F>(f))) {}

    TFunction & operator=(const TFunction & other) {
        if (this != &other) {
            callable_ = other.callable_->Clone();
        }
        return *this;
    }

    TFunction & operator=(TFunction && other) {
        if (this != &other) {
            callable_ = std::move(other.callable_);            
            other.callable_ = nullptr;
        }
        return *this;
    }

    Ret operator()(Args... args) const {
        if (!callable_) {
            throw std::bad_function_call();
        }
        return callable_->Invoke(std::forward<Args>(args)...);
    }

    bool IsValid() const {
        return static_cast<bool>(callable_);
    }

    bool IsEmpty() const {
        return !IsValid();
    }

private:

    std::unique_ptr<CallableBase> callable_;

};

template<std::size_t B, typename T>
struct TFnLmbd;

/**
 * @brief Optimized for lambdas, valid for function ptr too.
 */
template<std::size_t B, typename R, typename ...Args>
struct TFnLmbd<B, R(Args...)> {
public:
    
    template<typename T>
    static constexpr bool is_callable_v = requires (T && t, Args && ... args) {
        { std::forward<T>(t)(std::forward<Args>(args)...) } -> std::convertible_to<R>;
    };

    template<typename T>
    static constexpr bool in_size_v = sizeof(T) <= B;

    using ManageFn = R(*)(const void*, Args && ...);
    using ManageDstry = void(*)(const void*);
    using ManageCpy = void(*)(void* _dst,void* _src);
    using ManageMove = void(*)(void* _dst, void* _src);

    template<typename T>
    struct FnLmbdMan {
        static constexpr R managefn(const void* ptr, Args && ... args) {
            return (*(std::launder(reinterpret_cast<const T*>(ptr))))(
                std::forward<Args>(args)...
                );
        }

        static constexpr void managedstry(const void* ptr) {
            std::launder(reinterpret_cast<const T*>(ptr))->~T();
        }

        static void managecpy(void* dst, void * src) {
            (*std::launder(reinterpret_cast<T*>(dst))) = (*std::launder(reinterpret_cast<T*>(src)));
        }

        static void managemove(void* dst, void * src) {
            (*std::launder(reinterpret_cast<T*>(dst))) = std::move(
                (*std::launder(reinterpret_cast<T*>(src)))
                );
        }
    };

    constexpr TFnLmbd() noexcept = default;

    template<typename T> requires is_callable_v<T> && in_size_v<T>
    constexpr TFnLmbd(const T & fn) :
        managefn_(&FnLmbdMan<T>::managefn),
        managedstry_(&FnLmbdMan<T>::managedstry),
        managecpy_(&FnLmbdMan<T>::managecpy),
        managemove_(&FnLmbdMan<T>::managemove)
        {
            new (bf_) T(fn);
        }

    template<typename T> requires is_callable_v<T> && in_size_v<T>
    constexpr TFnLmbd(T && fn) :
        managefn_(&FnLmbdMan<T>::managefn),
        managedstry_(&FnLmbdMan<T>::managedstry),
        managecpy_(&FnLmbdMan<T>::managecpy),
        managemove_(&FnLmbdMan<T>::managemove)
        {
            new (bf_) T(std::move(fn));
        }

    constexpr TFnLmbd(const TFnLmbd & other) :
        managefn_(other.managefn_),
        managedstry_(other.managedstry_),
        managecpy_(other.managecpy_),
        managemove_(other.managemove_)
        {
            managecpy_(bf_, other.bf_);
        }

    constexpr TFnLmbd(TFnLmbd && other) noexcept :
        managefn_(std::move(other.managefn_)),
        managedstry_(std::move(other.managedstry_)),
        managecpy_(std::move(other.managecpy_)),
        managemove_(std::move(other.managemove_))
        {
            managemove_(bf_, other.bf_);

            other.managefn_=nullptr;
            other.managedstry_ = nullptr;
            other.managecpy_ = nullptr;
            other.managemove_ = nullptr;
        }

    TFnLmbd & operator=(const TFnLmbd & other) {
        if(this != &other) {
            managefn_ = other.managefn_;
            managedstry_ = other.managedstry_;
            managecpy_ = other.managecpy_;
            managemove_ = other.managemove_;

            managecpy_(bf_, other.bf_);
        }
        return *this;
    }

    TFnLmbd & operator=(TFnLmbd && other) {
        if (this != &other) {
            managefn_ = std::move(other.managefn_);
            managedstry_ = std::move(other.managedstry_);
            managecpy_ = std::move(other.managecpy_);
            managemove_ = std::move(other.managemove_);

            managemove_(bf_, other.bf_);

            other.managefn_=nullptr;
            other.managedstry_=nullptr;
            other.managecpy_=nullptr;
            other.managemove_=nullptr;
        }

        return *this;
    }

    constexpr virtual ~TFnLmbd() {
        if (managedstry_) {
            managedstry_(bf_);
        }
    }

    constexpr R operator()(Args && ... args) const {
        return Invoke(std::forward<Args>(args)...);
    }

    bool IsEmpty() const {
        return !IsValid();
    }

    bool IsValid() const {
        return static_cast<bool>(managefn_);
    }

private:


    constexpr R Invoke(Args && ... args) const {
        return (*static_cast<ManageFn>(managefn_)) (bf_, std::forward<Args>(args)...);
    }

    ManageFn managefn_;
    ManageDstry managedstry_;
    ManageCpy managecpy_;
    ManageMove managemove_;

    alignas(std::max_align_t) uint8_t bf_[B];
};

/** @breif 2Bytes lambda size */
template<typename R, typename ... Args>
using TFnLmbd2 = TFnLmbd<2, R(Args...)>;

/** @brief 4Bytes lambda size */
template<typename R, typename ... Args>
using TFnLmbd4 = TFnLmbd<4, R(Args...)>;

/** @brief 8Bytes lambda size, Also valid for function pointers. */
template<typename R, typename ... Args>
using TFnLmbd8 = TFnLmbd<8, R(Args...)>;

/** @brief 16Bytes lambda size */
template<typename R, typename ... Args>
using TFnLmbd16 = TFnLmbd<16, R(Args...)>;

/** @brief 32Bytes lambda size */
template<typename R, typename ...Args>
using TFnLmbd32 = TFnLmbd<32, R( Args...)>;

/** @brief 64bytes lambda size */
template<typename R, typename ... Args>
using TFnLmbd64 = TFnLmbd<64, R(Args...)>;


template<typename T>
struct TFnPtr;

/**
 * @brief Optimized for function pointers. Only valid for function ptrs.
 */
template<typename Ret, typename ...Args>
struct TFnPtr<Ret(Args...)> {

    constexpr TFnPtr() = default;

    constexpr virtual ~TFnPtr() = default;

    constexpr TFnPtr(Ret(*in_fn)(Args...)) : fn_(in_fn) {}

    constexpr TFnPtr(const TFnPtr & other) : fn_(other.fn_) {}

    constexpr TFnPtr(TFnPtr && other) : fn_(std::move(other.fn_)) { other.fn_ = nullptr; }

    constexpr TFnPtr & operator=(const TFnPtr & other) {
        if (this != &other) {
            fn_ = other.fn_;
        }
        return *this;
    }

    constexpr TFnPtr & operator=(TFnPtr && other) {
        if (this != &other) {
            fn_ = std::move(other.fn_);
            other.fn_ = nullptr;
        }
        return *this;
    }

    Ret operator()(Args... args) const {
        if (!fn_) {
            throw std::bad_function_call();
        }
        return fn_(args...);
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
