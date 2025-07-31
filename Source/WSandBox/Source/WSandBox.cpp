#include <type_traits>
#include <unordered_map>
#include <print>
#include <memory>
#include <vector>
#include <concepts>
#include <functional>

// #include "WCore/TFunction.hpp"

template<std::size_t B, typename T>
struct TFnLmbd;

template<std::size_t B, typename R, typename ...Args>
struct TFnLmbd<B, R(Args...)> {

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

    constexpr R Invoke(Args && ... args) const {
        return (*static_cast<ManageFn>(managefn_)) (bf_, std::forward<Args>(args)...);
    }

    ManageFn managefn_;
    ManageDstry managedstry_;
    ManageCpy managecpy_;
    ManageMove managemove_;

    alignas(std::max_align_t) uint8_t bf_[B];
};

template<typename T, typename ...Args>
using TFnLmbd32 = TFnLmbd<32, T( Args...)>;

template<typename T, typename ... Args>
using TFnLmbd64 = TFnLmbd<64, T(Args...)>;

template<typename T, typename ...Args>
using TFnLmbd128 = TFnLmbd<128, T( Args...)>;

template<typename T, typename ...Args>
using TFnLmbd256 = TFnLmbd<256, T( Args...)>;



void MFun() {
    std::print("MFun\n");
}

struct A {
    void operator()(int i) {}

    void operator()(int i, int l){}
};

int main(int argc, char* argv[])
{
    // TFunction<void()> fn([](){ MFun(); });

    // fn();

    // TFunPtr<void()> fnp (&MFun);

    // std::function<void()> fn2(MFun);

    // fn2();

    // static_cast<void(A::*)(int,int)>(&A::operator());

    TFnLmbd<64, int()> flm( [](){ std::print("I'm a lambda!\n"); return 2; } );

    flm();

    TFnLmbd<8, void()> fpt(&MFun);

    fpt();
    // void(*ptr)() = MFun;

    return 0;

}

