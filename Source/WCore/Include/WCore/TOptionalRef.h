#pragma once

#include "WCore/WCore.h"
#include "WCore/TFunction.h"

/**
 * Optional Reference Template
*/
template<typename T>
struct TOptionalRef
{

public:
    
    TOptionalRef() = default;

    TOptionalRef(T *object)
        : object(object)
    {
    }

    TOptionalRef(T &other)
        : object(&other)
    {
    }

    TOptionalRef(const TOptionalRef &other)
        : object(other.object)
    {
    }

    TOptionalRef(TOptionalRef&& other)
        : object(other.object)
    {
        other.object = nullptr;
    }

    TOptionalRef &operator=(T* other)
    {
        object = other;
        return *this;
    }

    TOptionalRef &operator=(T& other)
    {
        object = &other;
        return *this;
    }

    TOptionalRef &operator=(const TOptionalRef& other)
    {
        object = other.object;
        return *this;
    }

    TOptionalRef& operator=(TOptionalRef&& other)
    {
        object = other.object;
        other.object = nullptr;
        return *this;
    }

    bool operator==(const TOptionalRef& other) const
    {
        return object == other.object;
    }

    bool operator!=(const TOptionalRef& other) const
    {
        return object != other.object;
    }

    bool operator==(T* other) const
    {
        return object == other;
    }

    bool operator!=(T* other) const
    {
        return object != other;
    }

    T * operator->()
    {
        return object;
    }

    const T * operator->() const
    {
        return object;
    }

    T & operator*() {
        return *object;
    }

    const T& operator*() const
    {
        return *object;
    }

    operator bool() const
    {
        return !IsEmpty();
    }

    bool IsEmpty() noexcept { return object == nullptr; }

    T & Get() { return *object; }

    const T & Get() const noexcept { return *object; }

    // fill with IfEmpty IfEmptyOrElse 

    void IfEmpty(TFunction<void()> p) {
        if (IsEmpty()) { p(); }
    }

    void IfEmptyOrElse(TFunction<void()> p1, TFunction<void(T & in_item)> p2) {
        if (IsEmpty()) {
            p1();
        }
        else
        {
            p2(Get());
        }
    }

private:

    T * object = nullptr;


};
