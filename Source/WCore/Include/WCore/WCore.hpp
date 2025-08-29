#pragma once

#include "DllDef.hpp"
#include "WCore/WCoreMacros.hpp"

#include <cstring>
#include <functional>
#include <string_view>
#include <cstdint>
#include <cassert>
#include <type_traits>

#include "WCore.WEngine.hpp"

/**
 * @brief Basic identifier token
*/
template<typename T>
class WCORE_API _WId
{
public:

    using IdType = T;

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

template<typename T>
concept CIsWId = std::is_same_v<T, _WId<typename T::IdType>>;

using WId = _WId<std::size_t>;

using WLevelId = _WId<std::uint16_t>;
using WEntityId = _WId<std::uint32_t>;
using WComponentTypeId = _WId<std::uint8_t>;
using WEntityComponentId = _WId<std::size_t>;  // WLevelId[16] | WEntityId[32] | WComponentTypeId[8] | index[4]

using WAssetId = _WId<std::uint32_t>;
using WAssIdxId = _WId<std::uint8_t>;
using WAssetIndexId = _WId<std::uint64_t>; // WAssetId[32] | Index[4] (StaticMesh with material ids)

using WEventId = _WId<std::uint32_t>;

using WSystemId = _WId<std::uint16_t>;

using WLevelSystemId = _WId<std::uint32_t>;

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
                                      WComponentTypeId & out_dst3,
                                      WAssIdxId & out_dst4
        ) {
        std::size_t value = in_src.GetId();
        
        std::uint16_t lvlid=0;
        std::uint32_t entid=0;
        std::uint8_t cclassid=0;

        std::uint8_t index=0;
        std::uint8_t imask = 8 + 4 + 2 + 1;

        index |= (imask & value);
        value >>= 4;
        cclassid |= value;
        value >>= sizeof(WComponentTypeId) * 8;
        entid |= value;
        value >>= sizeof(WEntityId) * 8;
        lvlid |= value;

        out_dst1 = lvlid;
        out_dst2 = entid;
        out_dst3 = cclassid;
        out_dst4 = index;
    }

    inline WEntityComponentId ToEntityComponentId(const WLevelId & in_src1,
                                                  const WEntityId & in_src2,
                                                  const WComponentTypeId & in_src3,
                                                  const WAssIdxId & in_src4
        ) {
        size_t v=0;

        v |= in_src1.GetId();
        v <<= sizeof(WEntityId) * 8;
        v |= in_src2.GetId();
        v <<= sizeof(WComponentTypeId) * 8;
        v |= in_src3.GetId();
        v <<= 4;
        std::uint8_t imask = 8 + 4 + 2 + 1;
        v |= (imask & in_src4.GetId());

        return v;
    }

    inline void FromLevelSystemId(const WLevelSystemId & in_src,
                                  WLevelId & out_dst1,
                                  WSystemId & out_dst2) {
        std::uint32_t v = in_src.GetId();
        std::uint16_t lvlid = 0;
        std::uint16_t sysid = 0;

        sysid |= v;
        v >>= sizeof(WSystemId) * 8;
        lvlid |= v;

        out_dst1 = lvlid;
        out_dst2 = sysid;
    }

    inline WLevelSystemId ToLevelSystemId(const WLevelId & in_level_id,
                                          const WSystemId & in_system_id) {
        std::uint32_t v=0;
        v |= in_level_id.GetId();
        v <<= sizeof(WSystemId) * 8;
        v |= in_system_id.GetId();

        return v;
    }

    inline void FromAssetIndexId(const WAssetIndexId & in_asset_index_id,
                                 WAssetId & out_asset_id,
                                 WAssIdxId & out_index) {
        
        std::uint64_t v = in_asset_index_id.GetId();

        std::uint8_t aidval=0;
        std::uint32_t assval=0;

        std::uint8_t bitmask = 8 | 4 | 2 | 1;
        
        aidval |= (bitmask & v);
        
        v >>= 4;

        assval |= v;

        out_asset_id = assval;
        out_index = aidval;
    }

    inline WAssetIndexId ToAssetIndexId(const WAssetId & in_asset_id,
                                        const WAssIdxId & in_index) {

        assert(in_index.GetId() < WENG_MAX_ASSET_IDS);

        std::uint64_t result=0;
        
        result |= in_asset_id.GetId();
        
        result <<= 4;

        std::uint8_t bitmask = 8 | 4 | 2 | 1;

        result |= (bitmask & in_index.GetId());

        return result;
    }

}
