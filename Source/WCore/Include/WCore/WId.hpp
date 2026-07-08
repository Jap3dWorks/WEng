#pragma once

#include "LinkDef.hpp"

#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <limits>
#include <cassert>

struct _WID_NULL_T_{};
inline constexpr _WID_NULL_T_ const wid_null{};

struct WIdDefaultFlag{};

/**
 * @brief Basic identifier token
 * TODO: configurable null value.
 * TODO: namespace.
*/
template<typename T=std::size_t, typename Flag=WIdDefaultFlag, T NullValue=0>
class WCORE_API WId
{
public:

    using IdType = T;
    using FlagType = Flag;

    static inline constexpr T NULL_VALUE{NullValue};

public:

    constexpr WId() noexcept = default;
    constexpr WId(const WId&) noexcept = default;
    constexpr WId(WId&&) noexcept = default;
    constexpr WId& operator=(const WId&) noexcept = default;
    constexpr WId& operator=(WId&&) noexcept = default;
    virtual ~WId() = default;

    constexpr WId(T id) noexcept :
    id_(id) {}

    constexpr WId(_WID_NULL_T_) noexcept :
    id_(NULL_VALUE) {}

    constexpr T GetId() const noexcept {
        return id_;
    }

    constexpr T operator*() const noexcept {
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

    constexpr operator T() const noexcept {
        return id_;
    }

    constexpr bool IsValid() const noexcept {
        return id_ != NULL_VALUE;
    }

    constexpr operator bool() const noexcept {
        return IsValid();
    }

protected:

    T id_{NULL_VALUE}; 

};

template<typename T>
concept CIsWId =
    std::is_same_v<T, WId<typename T::IdType, typename T::FlagType, T::NULL_VALUE >> ||
    std::is_base_of_v<WId<typename T::IdType, typename T::FlagType, T::NULL_VALUE >, T>;

namespace std
{
    template <CIsWId T>
    struct hash<T>
    {
        std::size_t operator()(T const & in_wid) const
        {
            return std::hash<typename T::IdType>{}(in_wid.GetId());
        }
    };
}

// ---------------
// Specialized IDs
// ---------------

struct _WAssetId_Flag_{};
using WAssetId = WId<std::uint32_t,
                     _WAssetId_Flag_>;

struct _WSubIdxId_Flag_{};
using WSubIdxId = WId<std::uint8_t,
                      _WSubIdxId_Flag_,
                      std::numeric_limits<std::uint8_t>::max()>;

struct _WEntityId_Flag_{};
using WEntityId = WId<std::uint32_t,
                      _WEntityId_Flag_>;

struct _WComponentTypeId_Flag_{};
using WComponentTypeId = WId<std::uint8_t,
                             _WComponentTypeId_Flag_
                             >;

struct _WEventId_Flag_{};
using WEventId = WId<std::uint32_t, _WEventId_Flag_>;

struct _WSystemId_Flag_{};
using WSystemId = WId<std::uint16_t, _WSystemId_Flag_>;

// TODO Compound id class
struct _WLevelSustemId_Flag_{};
using WLevelSystemId = WId<std::uint32_t, _WLevelSustemId_Flag_>;

struct _WRenderId_Flag_{};

/**
 * 0 disabled. Each id is a render flow.
 * WRenderId 1 is the common render flow.
 */
using WRenderId = WId<std::uint8_t, _WRenderId_Flag_>;

template<typename T>
concept WEntityComponentId_Subtype = std::is_same_v<T,WAssetId> ||
    std::is_same_v<T,WEntityId> ||
    std::is_same_v<T,WComponentTypeId> ||
    std::is_same_v<T,WSubIdxId>;


// -----------------
// Compound ID Types
// -----------------


namespace {
    inline constexpr std::size_t GenBitMask(std::uint8_t bits_size) {
        return ~(std::numeric_limits<std::size_t>::max() <<  bits_size);
    }

    template<CIsWId T>
    inline constexpr bool IsValidWid(T val, std::uint8_t bits_size) {
        return (val.GetId() & ~GenBitMask(bits_size)) == 0;
    }

    template<std::size_t ... Sizes, typename ...WIdTypes>
    inline void ValidateWIds() {}

    template<std::size_t N, std::size_t ... Sizes, typename WIdType, typename ...WIdTypes>
    inline void ValidateWIds(WIdType id_type, WIdTypes ... id_types ) {

        static_assert(sizeof...(Sizes) == sizeof...(WIdTypes));

        if (IsValidWid(id_type, N)) {
            ValidateWIds<Sizes...>(id_types...);
        }
        else {
            throw std::runtime_error(
                "Invalid IDs values!"
                );
        }
    }
}

// ------------------
// WEntityComponentId
// ------------------

struct _WEntityComponentId_Flag_{};

/**
 * WEntityComponentId id class.
 */
class WCORE_API WEntityComponentId : public WId<std::uint64_t, _WEntityComponentId_Flag_> {
public:
    
    using WId::WId;

    static constexpr std::uint8_t LEVEL_BITS_SIZE{16};
    static constexpr std::uint8_t ENTITY_BITS_SIZE{22};
    static constexpr std::uint8_t COMPONENT_BITS_SIZE{8};
    static constexpr std::uint8_t SUBINDEX_BITS_SIZE{5};

    static_assert(LEVEL_BITS_SIZE +
                  ENTITY_BITS_SIZE +
                  COMPONENT_BITS_SIZE +
                  SUBINDEX_BITS_SIZE <= 62);    

    template<WEntityComponentId_Subtype T>
    static constexpr std::uint8_t GetBitsSize() {
        if constexpr(std::is_same_v<T, WAssetId>) {
            return LEVEL_BITS_SIZE;
        }
        else if constexpr (std::is_same_v<T, WEntityId>) {
            return ENTITY_BITS_SIZE;
        }
        else if constexpr (std::is_same_v<T, WComponentTypeId>) {
            return COMPONENT_BITS_SIZE;
        }
        else {
            return SUBINDEX_BITS_SIZE;
        }
    }

    template<WEntityComponentId_Subtype T>
    static constexpr std::uint8_t BitsSizeV = GetBitsSize<T>();

    template<WEntityComponentId_Subtype T>
    static constexpr std::size_t BitMaskV = GenBitMask(BitsSizeV<T>);

public:

    constexpr WEntityComponentId() noexcept = default;
    constexpr WEntityComponentId(const WEntityComponentId&) noexcept = default;
    constexpr WEntityComponentId(WEntityComponentId&&) noexcept = default;
    constexpr WEntityComponentId& operator=(const WEntityComponentId&) noexcept = default;
    constexpr WEntityComponentId& operator=(WEntityComponentId&&) noexcept = default;
    virtual ~WEntityComponentId() = default;

    WEntityComponentId(WAssetId in_asset_id,
                          WEntityId in_entity_id,
                          WComponentTypeId in_component_id,
                          WSubIdxId in_subIdx_id) {


        ValidateWIds<
            BitsSizeV<WAssetId>,
            BitsSizeV<WEntityId>,
            BitsSizeV<WComponentTypeId>,
            BitsSizeV<WSubIdxId>>
            (
                in_asset_id, in_entity_id, in_component_id, in_subIdx_id
                );

        id_=0;

        id_ |= BitMaskV<WAssetId> & in_asset_id.GetId();

        id_ <<= BitsSizeV<WEntityId>;
        id_ |=  BitMaskV<WEntityId> & in_entity_id.GetId();

        id_ <<= BitsSizeV<WComponentTypeId>;
        id_ |= BitMaskV<WComponentTypeId> & in_component_id.GetId();

        id_ <<= BitsSizeV<WSubIdxId>;
        id_ |= BitMaskV<WSubIdxId> & in_subIdx_id.GetId();
    }

    void ExtractWIds(WAssetId & out_asset_id,
                     WEntityId & out_entity_id,
                     WComponentTypeId & out_component_id,
                     WSubIdxId & out_subidx_id) const {

        IdType idcpy = id_;

        WAssetId::IdType asset_id = 0;
        WEntityId::IdType entity_id = 0;
        WComponentTypeId::IdType component_id = 0;
        WSubIdxId::IdType subidx_id = 0;

        subidx_id |= BitMaskV<WSubIdxId> & idcpy;
        idcpy >>= BitsSizeV<WSubIdxId>;

        component_id |= BitMaskV<WComponentTypeId> & idcpy;
        idcpy >>= BitsSizeV<WComponentTypeId>;

        entity_id |= BitMaskV<WEntityId> & idcpy;
        idcpy >>= BitsSizeV<WEntityId>;

        asset_id |= BitMaskV<WAssetId> & idcpy;

        out_asset_id = asset_id;
        out_entity_id = entity_id;
        out_component_id = component_id;
        out_subidx_id = subidx_id;
    }

};


// -------------
// WAssetIndexId
// -------------

template<typename T>
concept WAssetIndexId_Subtype = std::is_same_v<T, WAssetId> ||
    std::is_same_v<T, WSubIdxId>;

struct _WAssetIndexId_Flag_{};

class WCORE_API WAssetIndexId : public WId<std::uint64_t, _WAssetIndexId_Flag_> {
public:
    using WId::WId;

    static constexpr std::uint8_t ASSET_BITS_SIZE{sizeof(WAssetId::IdType) * 8};
    static constexpr std::uint8_t SUBINDEX_BITS_SIZE{5};

    static_assert(ASSET_BITS_SIZE + SUBINDEX_BITS_SIZE <= 62);

    template<WAssetIndexId_Subtype T>
    static constexpr std::uint8_t GetBitsSize(){
        if constexpr(std::is_same_v<T, WAssetId>) {
            return ASSET_BITS_SIZE;
        }
        else {
            return SUBINDEX_BITS_SIZE;
        }
    }

    template<WAssetIndexId_Subtype T>
    static constexpr std::uint8_t BitsSizeV = GetBitsSize<T>();

    template<WAssetIndexId_Subtype T>
    static constexpr std::size_t BitMaskV = GenBitMask(BitsSizeV<T>);

public:

    constexpr WAssetIndexId() noexcept = default;
    constexpr WAssetIndexId(const WAssetIndexId&) noexcept = default;
    constexpr WAssetIndexId(WAssetIndexId&&) noexcept = default;
    constexpr WAssetIndexId& operator=(const WAssetIndexId&) noexcept = default;
    constexpr WAssetIndexId& operator=(WAssetIndexId&&) noexcept = default;
    virtual ~WAssetIndexId() = default;

    WAssetIndexId(WAssetId in_asset_id, WSubIdxId in_subidx) {

        ValidateWIds<
            BitsSizeV<WAssetId>,
            BitsSizeV<WSubIdxId>>
            (
                in_asset_id,
                in_subidx
                );

        id_ = 0;

        id_ |= BitMaskV<WAssetId> & in_asset_id.GetId();

        id_ <<= BitsSizeV<WSubIdxId>;
        id_ |= BitMaskV<WSubIdxId> & in_asset_id.GetId();
    }

    void ExtractWIds(WAssetId & out_asset_id, WSubIdxId & out_subidx) const {
        IdType idcpy = id_;

        WAssetId::IdType asset_id = 0;
        WSubIdxId::IdType subidx = 0;

        subidx |= BitMaskV<WSubIdxId> & idcpy;
        idcpy >>= BitsSizeV<WSubIdxId>;

        asset_id |= BitMaskV<WAssetId> & idcpy;

        out_asset_id = asset_id;
        out_subidx = subidx;
    }
};

// --------------
// WLevelSystemId
// --------------
template<typename T>
concept WLevelSystemId_Subtype =
    std::is_same_v<T, WAssetId> ||
    std::is_same_v<T, WSystemId>;

struct _N_WLevelSustemId_Flag_{};

class N_WLevelSystemId : public WId<std::uint64_t, _N_WLevelSustemId_Flag_> {
public:
    using WId::WId;

    static constexpr std::uint8_t LEVEL_BITS_SIZE{16};
    static constexpr std::uint8_t SYSTEM_BITS_SIZE{16};

    static_assert(LEVEL_BITS_SIZE + SYSTEM_BITS_SIZE < 62);

    template<WLevelSystemId_Subtype T>
    static constexpr std::uint8_t GetBitsSize() {
        if constexpr(std::is_same_v<T,WAssetId>) {
            return LEVEL_BITS_SIZE;
        }
        else {
            return SYSTEM_BITS_SIZE;
        }
    } 

    template<WLevelSystemId_Subtype T>
    static constexpr std::uint8_t BitsSizeV = GetBitsSize<T>();

    template<WLevelSystemId_Subtype T>
    static constexpr std::size_t BitMaskV = GenBitMask(BitsSizeV<T>);

public:

    constexpr N_WLevelSystemId() noexcept = default;
    constexpr N_WLevelSystemId(N_WLevelSystemId const &) noexcept = default;
    constexpr N_WLevelSystemId(N_WLevelSystemId &&) noexcept = default;
    constexpr N_WLevelSystemId& operator=(N_WLevelSystemId const &) noexcept = default;
    constexpr N_WLevelSystemId& operator=(N_WLevelSystemId &&) noexcept = default;
    virtual ~N_WLevelSystemId() = default;

    N_WLevelSystemId(WAssetId in_asset_id, WSystemId in_system_id) {
        
        ValidateWIds<
            BitsSizeV<WAssetId>,
            BitsSizeV<WSystemId>
            > (
            in_asset_id, in_system_id
            );


        id_ = 0;

        id_ |= BitMaskV<WAssetId> & in_asset_id.GetId();

        id_ <<= BitsSizeV<WSystemId>;
        id_ |= BitMaskV<WSystemId> & in_system_id.GetId();
    }

    void ExtractWIds(WAssetId out_asset_id, WSystemId out_system_id) const {
        IdType idcpy = id_;

        WAssetId::IdType asset_id=0;
        WSystemId::IdType system_id=0;

        system_id |= BitMaskV<WSystemId> & idcpy;
        idcpy >>= BitsSizeV<WSystemId>;

        asset_id |= BitMaskV<WAssetId> ^ idcpy;

        out_asset_id = asset_id;
        out_system_id = system_id;

    }

};

// ------
// WEngId
// ------

enum class EObjectKind : std::uint8_t {
    Asset         = 0b01,
    EntityComponent = 0b10,
    System          = 0b11
    // 0b00 reserved
};

/**
 * WEngId General id class
 */
class WCORE_API WEngId {

    static constexpr std::uint64_t NullSentinel {0ULL};
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
        std::uint64_t payload = ecId.GetId();
        return WEngId(
            (static_cast<uint64_t>(EObjectKind::EntityComponent) << KindSifht) |
            payload );
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

