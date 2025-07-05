#pragma once

#include "DllDef.h"
#include "WCore/CoreMacros.hpp"

#include <cstring>
#include <string>
#include <functional>
#include <string_view>
#include <cassert>

#include "WCore.intermediate.h"


/**
 * @brief Basic identifier token
*/
class WCORE_API WId
{
public:

    constexpr WId() noexcept :
    id_(0) {}

    constexpr WId(size_t id) noexcept :
    id_(id) {}

    constexpr size_t GetId() const noexcept {
        return id_;
    }

    constexpr bool operator==(const WId &other) const noexcept {
        return id_ == other.id_;
    }
    
    constexpr bool operator!=(const WId &other) const noexcept {
        return id_ != other.id_;
    }

    constexpr bool operator<(const WId &other) const noexcept {
        return id_ < other.id_;
    }
    
    constexpr bool operator>(const WId &other) const noexcept {
        return id_ > other.id_;
    }

    constexpr bool operator<=(const WId &other) const noexcept {
        return id_ <= other.id_;
    }
    
    constexpr bool operator>=(const WId &other) const noexcept {
        return id_ >= other.id_;
    }

    constexpr WId operator+(const WId &other) const noexcept {
        return WId(id_ + other.id_);
    }
    
    constexpr WId operator-(const WId &other) const noexcept {
        return WId(id_ - other.id_);
    }

    constexpr WId& operator++() noexcept
    {
        ++id_;
        return *this;
    }

    constexpr WId operator++(int) noexcept
    {
        WId temp = WId(id_);
        ++id_;
        return temp;
    }

    constexpr WId& operator--() noexcept
    {
        --id_;
        return *this;
    }

    constexpr WId operator--(int) noexcept
    {
        WId temp = WId(id_);
        --id_;
        return temp;
    }

    constexpr WId& operator+=(const WId &other) noexcept
    {
        id_ += other.id_;
        return *this;
    }

    constexpr WId& operator-=(const WId &other) noexcept
    {
        id_ -= other.id_;
        return *this;
    }

    constexpr operator size_t() const noexcept {
        return id_;
    }

    constexpr bool IsValid() const noexcept {
        return id_ > 0;
    }

    constexpr operator bool() const noexcept {
        return IsValid();
    }

private:

    size_t id_=0; 
};

namespace std
{
    template <>
    struct hash<WId>
    {
        std::size_t operator()(const WId & in_wid) const
        {
            return std::hash<std::size_t>{}(in_wid.GetId());
        }
    };
}


class WCORE_API WClass
{
public:

    constexpr WClass() noexcept = default;
    constexpr WClass(const char *name) noexcept :
    name_(name) {}

    virtual ~WClass() = default;

    constexpr const char *GetName() const noexcept {
        return name_;
    }

public:

    constexpr bool operator==(const WClass &other) const noexcept
    {
        return std::string_view(name_) == other.name_;
    }
    
    constexpr bool operator!=(const WClass &other) const noexcept
    {
        return std::string_view(name_) != other.name_;
    }

    constexpr bool operator<(const WClass &other) const noexcept
    {
        return std::string_view(name_) < other.name_;
    }
    
    constexpr bool operator>(const WClass &other) const noexcept
    {
        return std::string_view(name_) > other.name_;
    }

    constexpr bool operator<=(const WClass &other) const noexcept
    {
        return std::string_view(name_) <= other.name_;
    }
    
    constexpr bool operator>=(const WClass &other) const noexcept
    {
        return std::string_view(name_) >= other.name_;
    }

private:
    const char * name_;
    
};

namespace std
{
    template <>
    struct hash<WClass>
    {
        std::size_t operator()(const WClass &wclass) const
        {
            return std::hash<std::string>{}(wclass.GetName());
        }

    };
}

WCLASS()
class WCORE_API WObject
{
    WOBJECT_BODY(WObject)

public:

    WNODISCARD constexpr WId WID() const noexcept
    { return wid_; }

    constexpr void WID(WId in_wid) noexcept {
        assert(wid_.GetId() == 0);
        wid_ = in_wid;
    }

protected:

    WId wid_;

private:

};

