#pragma once

#include "DllDef.hpp"
#include "WCore/WCoreMacros.hpp"

#include <cstring>
#include <functional>
#include <string_view>
#include <cstdint>

#include "WCore.WEngine.hpp"

/**
 * @brief Basic identifier token
*/
template<typename T>
class WCORE_API _WId
{
public:

    constexpr _WId() noexcept :
    id_(0) {}

    constexpr _WId(T id) noexcept :
    id_(id) {}

    constexpr T GetId() const noexcept {
        return id_;
    }

    constexpr bool operator==(const _WId &other) const noexcept {
        return id_ == other.id_;
    }
    
    constexpr bool operator!=(const _WId &other) const noexcept {
        return id_ != other.id_;
    }

    constexpr bool operator<(const _WId &other) const noexcept {
        return id_ < other.id_;
    }
    
    constexpr bool operator>(const _WId &other) const noexcept {
        return id_ > other.id_;
    }

    constexpr bool operator<=(const _WId &other) const noexcept {
        return id_ <= other.id_;
    }
    
    constexpr bool operator>=(const _WId &other) const noexcept {
        return id_ >= other.id_;
    }

    constexpr _WId operator+(const _WId &other) const noexcept {
        return _WId(id_ + other.id_);
    }
    
    constexpr _WId operator-(const _WId &other) const noexcept {
        return _WId(id_ - other.id_);
    }

    constexpr _WId& operator++() noexcept
    {
        ++id_;
        return *this;
    }

    constexpr _WId operator++(int) noexcept
    {
        _WId temp = _WId(id_);
        ++id_;
        return temp;
    }

    constexpr _WId& operator--() noexcept
    {
        --id_;
        return *this;
    }

    constexpr _WId operator--(int) noexcept
    {
        _WId temp = _WId(id_);
        --id_;
        return temp;
    }

    constexpr _WId& operator+=(const _WId &other) noexcept
    {
        id_ += other.id_;
        return *this;
    }

    constexpr _WId& operator-=(const _WId &other) noexcept
    {
        id_ -= other.id_;
        return *this;
    }

    constexpr operator T() const noexcept {
        return id_;
    }

    constexpr bool IsValid() const noexcept {
        return id_ > 0;
    }

    constexpr operator bool() const noexcept {
        return IsValid();
    }

private:

    T id_=0; 
};

using WId = _WId<std::size_t>;
using WEntityId = _WId<std::uint32_t>;
using WComponentTypeId = _WId<std::uint8_t>;
using WEntityComponentId = _WId<std::size_t>;  // combine entity with component type id;
using WAssetId = _WId<std::size_t>;

namespace std
{
    template <typename T>
    struct hash<_WId<T>>
    {
        std::size_t operator()(const WId & in_wid) const
        {
            return std::hash<T>{}(in_wid.GetId());
        }
    };
}

