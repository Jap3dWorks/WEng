#pragma once

#include "DllDef.hpp"
#include "WCore/WCoreMacros.hpp"

#include <cstring>
#include <string>
#include <functional>
#include <string_view>
#include <cstdint>

#include "WCore.WEngine.hpp"

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

