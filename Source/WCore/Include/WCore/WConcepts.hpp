#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

template<typename To, typename From>
concept CConvertibleTo = std::is_convertible_v<From*, To*>;

template<typename T>
concept CHasWid = requires(T a) { a.WID(); };

class WObject;

template<typename T>
concept CWObjectDerived = std::is_base_of_v<WObject, T>;

template<typename T, typename R, typename... Args>
concept CObjCallable = requires(T t, Args... args) {
    { t(args...) } -> std::same_as<R>;
};

template<typename T, typename R, typename ...Args>
concept CPtrFn = std::is_same_v<T , R(*)(Args...)>;

template<typename T, typename R, typename C, typename... Args>
concept CMemberFn = std::is_same_v<T, R(C::*)(Args...)> || 
                   std::is_same_v<T, R(C::*)(Args...) const>;

template<typename T, typename R, typename... Args>
concept CCallable = CObjCallable<T, R, Args...>  ||
    CPtrFn<T, R, Args...>        ||
    requires(T t, Args... args) {
        { t(args...) } -> std::convertible_to<R>;
    };

