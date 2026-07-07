#pragma once

#include "LinkDef.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WCoreTypes/WCoreTypes.hpp"

#include <cstring>
#include <functional>
#include <limits>
#include <string_view>
#include <cstdint>
#include <cassert>
#include <type_traits>

struct _WID_NULL_T_{};
inline constexpr _WID_NULL_T_ const wid_null{};

/**
 * @brief Basic identifier token
 * TODO: configurable null value.
 * TODO: namespace.
*/
template<typename T, T NullValue=0>
class WCORE_API _WId
{
public:

    using IdType = T;

    static inline constexpr T NULL_VALUE{NullValue};

public:

    constexpr _WId() noexcept =default;

    constexpr _WId(T id) noexcept :
    id_(id) {}

    constexpr _WId(_WID_NULL_T_) noexcept :
    id_(NULL_VALUE) {}

    constexpr T GetId() const noexcept {
        return id_;
    }

    constexpr T operator*() const noexcept {
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
        return id_ != NULL_VALUE;
    }

    constexpr operator bool() const noexcept {
        return IsValid();
    }

private:

    T id_{NULL_VALUE}; 

};



template<typename T>
concept CIsWId = std::is_same_v<T, _WId<typename T::IdType, T::NULL_VALUE >>;

using WId = _WId<std::size_t>;


// TODO Id Reestructuration
// -------------------

using WLevelId = _WId<std::uint16_t>;
using WEntityId = _WId<std::uint32_t>;
using WComponentTypeId = _WId<std::uint8_t>;
// WLevelId[22] | WEntityId[22] | WComponentTypeId[8] (WComponentID) (now it is runtime assigned) | index[5]
using WEntityComponentId = _WId<std::uint64_t>;

using WAssetId = _WId<std::uint32_t>;
using WSubIdxId = _WId<std::uint8_t, std::numeric_limits<std::uint8_t>::max()>;

// WAssetId[22] | Index[5] (StaticMesh with material ids)
using WAssetIndexId = _WId<std::uint64_t>;       


enum class EObjectKind : std::uint8_t {
    Asset         = 0b01,
    EntityComponent = 0b10,
    // 0b11 reserved
};

class WENGINE_API WEngId {

    static constexpr std::uint64_t NullSentinel {~0ULL};
    static constexpr std::uint64_t KindSifht = 62;
    static constexpr std::uint64_t KindMask  = 0b11ULL << KindSifht;

public:

    constexpr WEngId() = default;
    constexpr WEngId(const WEngId&) = default;
    constexpr WEngId(WEngId&&) = default;
    constexpr WEngId& operator=(const WEngId&) = default;
    constexpr WEngId& operator=(WEngId&&) = default;
    virtual ~WEngId() = default;

    static constexpr WEngId FromAsset(WAssetIndexId assetIndex) noexcept {
        std::uint64_t payload = assetIndex.GetId();
        return WEngId( (static_cast<std::uint64_t>(EObjectKind::Asset) << KindSifht) | payload );
    }

    static constexpr WEngId FromEntityComponent(WEntityComponentId ecId) noexcept {
        uint64_t payload = ecId.GetId();
        return WEngId( (static_cast<uint64_t>(EObjectKind::EntityComponent) << KindSifht) | payload );
    }

    constexpr bool IsValid() const noexcept { return data_ != NullSentinel; }

    constexpr EObjectKind Kind() const noexcept {
        return static_cast<EObjectKind>( (data_ & KindMask) >> KindSifht );
    }

    constexpr WAssetIndexId AsAssetIndexId() const noexcept {
        assert(Kind() == EObjectKind::Asset);
        return WAssetIndexId( data_ & ~KindMask );
    }

    constexpr WEntityComponentId AsEntityComponentId() const noexcept {
        assert(Kind() == EObjectKind::EntityComponent);
        return WEntityComponentId( data_ & ~KindMask );
    }

    constexpr explicit operator std::uint64_t() const noexcept { return data_; }
    static constexpr WEngId FromRaw(std::uint64_t raw) noexcept { return WEngId(raw); }

    constexpr bool operator==(const WEngId& o) const noexcept { return data_ == o.data_; }
    constexpr bool operator!=(const WEngId& o) const noexcept { return data_ != o.data_; }
    constexpr bool operator<(const WEngId& o) const noexcept { return data_ < o.data_; }

    struct Hash {
        std::size_t operator()(const WEngId& id) const noexcept {
            return std::hash<std::uint64_t>{}(id.data_);
        }
    };

private:

    constexpr explicit WEngId(std::uint64_t data) noexcept :
    data_(data) {}

    std::uint64_t data_{NullSentinel};

};

namespace std {
    template<>
    struct hash<WEngId> {
        std::size_t operator()(const WEngId& id) const noexcept {
            return WEngId::Hash{}(id);
        }
    };
}

// ------------------

using WEventId = _WId<std::uint32_t>;

using WSystemId = _WId<std::uint16_t>;

using WLevelSystemId = _WId<std::uint32_t>;

// 0 disabled. Each id is a render flow.
// WRenderId 1 is the common render flow.
using WRenderId = _WId<std::uint8_t>;

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
                                      WSubIdxId & out_dst4
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
                                                  const WSubIdxId & in_src4
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
                                 WSubIdxId & out_index) {
        
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
                                        const WSubIdxId & in_index) {

        assert(in_index.GetId() < WENG_MAX_ASSET_IDS);

        std::uint64_t result=0;
        
        result |= in_asset_id.GetId();
        
        result <<= 4;

        std::uint8_t bitmask = 8 | 4 | 2 | 1;

        result |= (bitmask & in_index.GetId());

        return result;
    }

}
