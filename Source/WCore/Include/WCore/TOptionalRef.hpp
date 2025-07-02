#pragma once

#include "WCore/TFunction.hpp"

struct NullOptionalRef_t
{
    enum class _Construct {_Token};
    explicit constexpr NullOptionalRef_t(_Construct) noexcept {}
};

inline constexpr NullOptionalRef_t null_optional_ref{NullOptionalRef_t::_Construct::_Token};

/**
 * Optional Reference Template
*/
template<typename T>
struct TOptionalRef
{

public:

    constexpr TOptionalRef(NullOptionalRef_t) noexcept {}

    constexpr TOptionalRef(T *object) noexcept
        : object(object)
    {
    }

    constexpr TOptionalRef(T &other) noexcept
        : object(&other)
    {
    }

    constexpr TOptionalRef(const TOptionalRef &other) noexcept
        : object(other.object)
    {
    }

    constexpr TOptionalRef(TOptionalRef&& other) noexcept
        : object(other.object)
    {
        other.object = nullptr;
    }

    constexpr TOptionalRef &operator=(T* other) noexcept
    {
        object = other;
        return *this;
    }

    constexpr TOptionalRef &operator=(T& other) noexcept
    {
        object = &other;
        return *this;
    }

    constexpr TOptionalRef &operator=(const TOptionalRef& other) noexcept
    {
        object = other.object;
        return *this;
    }

    constexpr TOptionalRef& operator=(TOptionalRef&& other) noexcept
    {
        object = other.object;
        other.object = nullptr;
        return *this;
    }

    constexpr bool operator==(const TOptionalRef& other) const noexcept
    {
        return object == other.object;
    }

    constexpr bool operator!=(const TOptionalRef& other) const noexcept
    {
        return object != other.object;
    }

    constexpr bool operator==(T* other) const noexcept
    {
        return object == other;
    }

    constexpr bool operator!=(T* other) const noexcept
    {
        return object != other;
    }

    constexpr T * operator->()
    {
        return object;
    }

    constexpr const T * operator->() const
    {
        return object;
    }

    constexpr T & operator*()
    {
        return *object;
    }

    constexpr const T & operator*() const
    {
        return *object;
    }

    constexpr operator bool() const
    {
        return !IsEmpty();
    }

    constexpr bool IsEmpty() noexcept { return object == nullptr; }

    constexpr bool IsPresent() noexcept { return !IsEmpty(); }

    constexpr T & Get() { return *object; }

    constexpr const T & Get() const noexcept { return *object; }

    // fill with IfEmpty IfEmptyOrElse 

    void IfEmpty(TFunction<void()> p) {
        if (IsEmpty()) { p(); }
    }

    void IfPresent(TFunction<void(T & item)> p)
    {
        if(IsPresent()) {
            p(Get());
        }
    }

    void IfPresentOrElse(TFunction<void(T & item)> p1, TFunction<void()> p2) {
        if (IsPresent()) {
            p1(Get());
        }
        else
        {
            p2();
        }
    }

private:

    T * object = nullptr;

};
