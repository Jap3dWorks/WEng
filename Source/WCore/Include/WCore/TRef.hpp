#pragma once

#include "WCore/WCore.hpp"
#include <stdexcept>

template<typename T>
struct TRef
{
    
public:

    TRef() = default;

    TRef(T * in_object) :
    object(in_object) {}

    TRef(T & in_object) :
    object(&in_object) {}

    TRef(const TRef & in_other) :
    object(in_other.object) {}

    TRef(TRef && in_other) :
    object(in_other.object) {
        in_other.object = nullptr;
    }

    TRef &operator=(T * other)
    {
        object = other;
        return *this;
    }

    TRef &operator=(T& other)
    {
        object = &other;
        return *this;
    }

    TRef &operator=(const TRef & other)
    {
        object = other.object;
        return *this;
    }

    TRef& operator=(TRef&& other)
    {
        object = other.object;
        other.object = nullptr;
        return *this;
    }

    bool operator==(const TRef & other) const
    {
        return object == other.object;
    }

    bool operator!=(const TRef & other) const
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
        return &Get();
    }

    const T * operator->() const
    {
        return &Get();
    }

    T & operator*()
    {
        return Get();
    }

    const T & operator*() const
    {
        return Get();
    }

    operator bool() const { return !IsEmpty(); }

    bool IsEmpty() noexcept { return object == nullptr; }

    T & Get() {
        if (!object) {
            throw std::runtime_error("Referencing to a null object");
        }
        
        return *object;
    } 

    const T & Get() const {
        if (!object) {
            throw std::runtime_error("Referencing to a null object");
        }
        return *object;
    }

private:
    
    T * object{nullptr};

};

