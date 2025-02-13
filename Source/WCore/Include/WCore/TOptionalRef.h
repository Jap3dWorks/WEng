#pragma once

/**
 * Optional Reference Template
*/
template<typename T>
struct TOptionalRef
{
    T *Object = nullptr;

    TOptionalRef() = default;

    TOptionalRef(T *Object)
        : Object(Object)
    {
    }

    TOptionalRef(T &Other)
        : Object(&Other)
    {
    }

    TOptionalRef(const TOptionalRef &Other)
        : Object(Other.Object)
    {
    }

    TOptionalRef(TOptionalRef&& Other)
        : Object(Other.Object)
    {
        Other.Object = nullptr;
    }

    TOptionalRef &operator=(T* Other)
    {
        Object = Other;
        return *this;
    }

    TOptionalRef &operator=(T& Other)
    {
        Object = &Other;
        return *this;
    }

    TOptionalRef &operator=(const TOptionalRef& Other)
    {
        Object = Other.Object;
        return *this;
    }

    TOptionalRef& operator=(TOptionalRef&& Other)
    {
        Object = Other.Object;
        Other.Object = nullptr;
        return *this;
    }

    operator T*() const
    {
        return Object;
    }

    T* operator->() const
    {
        return Object;
    }

    T& operator*() const
    {
        return *Object;
    }

    bool operator==(const TOptionalRef& Other) const
    {
        return Object == Other.Object;
    }

    bool operator!=(const TOptionalRef& Other) const
    {
        return Object != Other.Object;
    }


    bool operator==(T* Other) const
    {
        return Object == Other;
    }

    bool operator!=(T* Other) const
    {
        return Object != Other;
    }

    // bool operator==(std::nullptr_t) const
    // {
    //     return Object == nullptr;
    // }

    // bool operator!=(std::nullptr_t) const
    // {
    //     return Object != nullptr;
    // }

    operator bool() const
    {
        return Object != nullptr;
    }
};
