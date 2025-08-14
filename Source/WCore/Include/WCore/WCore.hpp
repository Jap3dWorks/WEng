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
using WEntityComponentId = _WId<std::size_t>;  // WLevelId | WEntityId | WComponentTypeId
using WAssetId = _WId<std::size_t>;
using WLevelId = _WId<std::uint16_t>;

using WEventId = _WId<std::uint32_t>;

using WSystemId = _WId<std::uint32_t>;

namespace std
{
    template <typename T>
    struct hash<_WId<T>>
    {
        std::size_t operator()(const _WId<T> & in_wid) const
        {
            return std::hash<T>{}(in_wid.GetId());
        }
    };
}

namespace WIdUtils {

    inline void FromEntityComponentId(const WEntityComponentId & in_src,
                                      WLevelId & out_dst1,
                                      WEntityId & out_dst2,
                                      WComponentTypeId & out_dst3) {
        std::size_t value = in_src.GetId();
        
        out_dst3 = value;
        value >>= sizeof(WComponentTypeId);
        out_dst2 = value;
        value >>= sizeof(WEntityId);
        out_dst1 = value;
    }

    inline WEntityComponentId ToEntityComponentId(const WLevelId & in_src1,
                                                  const WEntityId & in_src2,
                                                  const WComponentTypeId & in_src3) {
        size_t v=0;

        v |= in_src1.GetId();
        v <<= sizeof(WEntityId);
        v |= in_src2.GetId();
        v <<= sizeof(WComponentTypeId);
        v |= in_src3.GetId();

        return v;
    }

}
