#pragma once

#include <concepts>

template<typename To, typename From>
concept CConvertibleTo = std::is_convertible_v<From*, To*>;

template<typename T>
concept CHasWid = requires(T a) { a.WID(); };

class WObject;

template<typename T>
concept CWObjectDerived = std::is_base_of_v<WObject, T> && CHasWid<T>;

