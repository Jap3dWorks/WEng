#pragma once

// #include "WCore/WCore.hpp"
// #include <stdexcept>

#include <cassert>

template<typename T>
struct TRef
{
    
public:

    constexpr TRef() noexcept = default;

    constexpr TRef(T * in_object) noexcept :
    object_(in_object) {}

    constexpr TRef(T & in_object) noexcept :
    object_(&in_object) {}

    constexpr TRef(const TRef & in_other) noexcept :
    object_(in_other.object_) {}

    constexpr TRef(TRef && in_other) noexcept :
    object_(in_other.object_) {
        in_other.object_ = nullptr;
    }

    constexpr TRef & operator=(const TRef & other) noexcept {
        if (this != &other) {
            object_ = other.object_;            
        }

        return *this;
    }

    constexpr TRef & operator=(TRef&& other) noexcept {

        if (this != &other) {
            object_ = other.object_;
            other.object_ = nullptr;
            
        }
        return *this;
    }

    constexpr TRef & operator=(T * in_value) noexcept {
        object_ = in_value;
        return *this;
    }

    constexpr TRef & operator=(T & in_value) noexcept {
        object_ = &in_value;
        return *this;
    }

    TRef & operator=(const T & other) = delete;
    TRef & operator=(T && other) = delete;

    constexpr bool operator==(const TRef & other) const noexcept
    {
        return object_ == other.object_;
    }

    constexpr bool operator!=(const TRef & other) const noexcept
    {
        return object_ != other.object_;
    }


    constexpr bool operator==(T* other) const noexcept
    {
        return object_ == other;
    }

    constexpr bool operator!=(T* other) const noexcept
    {
        return object_ != other;
    }

    constexpr T * operator->() noexcept
    {
        return &Get();
    }

    constexpr const T * operator->() const noexcept
    {
        return &Get();
    }

    constexpr T & operator*() noexcept
    {
        return Get();
    }

    constexpr const T & operator*() const noexcept
    {
        return Get();
    }

    operator bool() const { return !IsEmpty(); }

    constexpr bool IsValid() const noexcept { return object_ != nullptr; }

    constexpr bool IsEmpty() const noexcept { return object_ == nullptr; }

    constexpr T & Get() noexcept {
        assert(object_);
        
        return *object_;
    } 

    constexpr const T & Get() const noexcept {
        assert(object_);

        return *object_;
    }

    constexpr T * Ptr() noexcept {
        return object_;
    }

    constexpr const T * Ptr() const noexcept {
        return object_;
    }

private:
    
    T * object_{nullptr};

};

