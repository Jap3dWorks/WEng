#pragma once

#include "LinkDef.hpp"

#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <limits>
#include <cassert>
#include <array>

namespace wid {
    
    struct _WID_NULL_T_{};
    inline constexpr _WID_NULL_T_ const WID_NULL_V{};

    struct WIdDefaultFlag{};

   /**
    * @brief Basic identifier token
    * TODO: namespace.
    */
    template<typename T=std::size_t, typename Flag=WIdDefaultFlag, T NullValue=0>
    class WCORE_API WId
    {
    public:

    using IdType = T;
    using FlagType = Flag;

    static inline constexpr T const NULL_VALUE{NullValue};

    public:

    constexpr WId() noexcept = default;
    constexpr WId(const WId&) noexcept = default;
    constexpr WId(WId&&) noexcept = default;
    constexpr WId& operator=(const WId&) noexcept = default;
    constexpr WId& operator=(WId&&) noexcept = default;
    ~WId() = default;

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

// ---------------
// Specialized IDs
// ---------------

    struct _WAssetId_Flag_{};
    using WAssetId = WId<std::uint32_t,
                         _WAssetId_Flag_>;

    struct _WAssetTypeId_Flag_{};
    using WAssetTypeId = WId<std::uint16_t,
                             _WAssetTypeId_Flag_,
                             std::numeric_limits<std::uint16_t>::max()>;

    struct _WSubIdxId_Flag_{};
    using WSubIdxId = WId<std::uint8_t,
                          _WSubIdxId_Flag_,
                          0b11111>; // using 5 bits null value is 11111

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

    struct _WRenderId_Flag_{};

/**
 * 0 disabled. Each id is a render flow.
 * WRenderId 1 is the common render flow.
 */
    using WRenderId = WId<std::uint8_t, _WRenderId_Flag_>;

// -----------------
// Compound ID Types
// -----------------

    namespace {

        inline constexpr std::uint8_t const WID_COMPOUND_BITS_MAX{61};

        inline constexpr std::size_t GenBitMask(std::uint8_t bits_size) {
            return ~(std::numeric_limits<std::size_t>::max() <<  bits_size);
        }

        template<CIsWId T>
        inline constexpr bool IsValidWid(T val, std::uint8_t bits_size) {
            T nullid{WID_NULL_V};
            return (val.GetId() & ~GenBitMask(bits_size)) ==
                (nullid.GetId() & ~GenBitMask(bits_size));
        }

        template<std::size_t ... Sizes, typename ...WIdTypes>
        inline void _ValidateWIds_() {}

        template<std::size_t N, std::size_t ... Sizes, typename WIdType, typename ...WIdTypes>
        inline void _ValidateWIds_(WIdType id_type, WIdTypes ... id_types ) {

            static_assert(sizeof...(Sizes) == sizeof...(WIdTypes));

            if (IsValidWid(id_type, N)) {
                _ValidateWIds_<Sizes...>(id_types...);
            }
            else {
                throw std::runtime_error(
                    "Invalid IDs values!"
                    );
            }
        }

        template<CIsWId T, std::uint8_t _size> 
        struct SWidSize{
            using Type = T;
            static constexpr std::uint8_t size{_size};
        };

        template<typename T>
        constexpr std::size_t GetNullId() {
            return T(WID_NULL_V).GetId();
        }

        template<typename... SWidSizes>
        constexpr std::size_t ComputeCompoundNull() {
            constexpr auto sizes =
                std::array<std::size_t, sizeof...(SWidSizes)>{ SWidSizes::size... };

            constexpr auto nulls =
                std::array<std::size_t, sizeof...(SWidSizes)>{ GetNullId<typename SWidSizes::Type>()... };

            std::size_t result = 0;
            std::size_t shift = 0;
            // iterate from last to first (rightmost to leftmost)
            for (std::size_t i = sizes.size(); i > 0; --i) {
                result |= (nulls[i-1] & ((std::size_t{1} << sizes[i-1]) - 1)) << shift;
                shift += sizes[i-1];
            }
            return result;
        }


    }

// ------------------
// WEntityComponentId
// ------------------

    template<typename T>
    concept WEntityComponentId_Subtype = std::is_same_v<T,WAssetId> ||
        std::is_same_v<T,WEntityId> ||
        std::is_same_v<T,WComponentTypeId> ||
        std::is_same_v<T,WSubIdxId>;

    struct WEntityComponentId_Meta{
        static constexpr std::uint8_t LEVEL_BITS_SIZE{16};
        static constexpr std::uint8_t ENTITY_BITS_SIZE{22};
        static constexpr std::uint8_t COMPONENT_BITS_SIZE{8};
        static constexpr std::uint8_t SUBINDEX_BITS_SIZE{5};

        static constexpr std::uint8_t WID_BITS_SIZE{
            LEVEL_BITS_SIZE + ENTITY_BITS_SIZE +
            COMPONENT_BITS_SIZE + SUBINDEX_BITS_SIZE
        };

        static_assert(WID_BITS_SIZE <= WID_COMPOUND_BITS_MAX);

        static constexpr std::uint64_t NULL_VALUE = ComputeCompoundNull<
            SWidSize<WAssetId, LEVEL_BITS_SIZE>,
            SWidSize<WEntityId, ENTITY_BITS_SIZE>,
            SWidSize<WComponentTypeId, COMPONENT_BITS_SIZE>,
            SWidSize<WSubIdxId, SUBINDEX_BITS_SIZE>>();

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

    };

    /**
     * WEntityComponentId id class.
     */
    class WCORE_API WEntityComponentId :
        public WId<std::uint64_t,
                   WEntityComponentId_Meta,
                   WEntityComponentId_Meta::NULL_VALUE> {
    public:
    
        using WId::WId;

        constexpr WEntityComponentId() noexcept = default;
        constexpr WEntityComponentId(const WEntityComponentId&) noexcept = default;
        constexpr WEntityComponentId(WEntityComponentId&&) noexcept = default;
        constexpr WEntityComponentId& operator=(const WEntityComponentId&) noexcept = default;
        constexpr WEntityComponentId& operator=(WEntityComponentId&&) noexcept = default;
        ~WEntityComponentId() = default;

        WEntityComponentId(WAssetId in_asset_id,
                           WEntityId in_entity_id,
                           WComponentTypeId in_component_id,
                           WSubIdxId in_subIdx_id) {

            _ValidateWIds_<
                WEntityComponentId_Meta::BitsSizeV<WAssetId>,
                WEntityComponentId_Meta::BitsSizeV<WEntityId>,
                WEntityComponentId_Meta::BitsSizeV<WComponentTypeId>,
                WEntityComponentId_Meta::BitsSizeV<WSubIdxId>>
                (
                    in_asset_id, in_entity_id, in_component_id, in_subIdx_id
                    );

            id_ = 0;

            id_ |= WEntityComponentId_Meta::BitMaskV<WAssetId> & in_asset_id.GetId();

            id_ <<= WEntityComponentId_Meta::BitsSizeV<WEntityId>;
            id_ |=  WEntityComponentId_Meta::BitMaskV<WEntityId> & in_entity_id.GetId();

            id_ <<= WEntityComponentId_Meta::BitsSizeV<WComponentTypeId>;
            id_ |= WEntityComponentId_Meta::BitMaskV<WComponentTypeId> & in_component_id.GetId();

            id_ <<= WEntityComponentId_Meta::BitsSizeV<WSubIdxId>;
            id_ |= WEntityComponentId_Meta::BitMaskV<WSubIdxId> & in_subIdx_id.GetId();
        }

        void ExtractWIds(WAssetId & out_asset_id,
                         WEntityId & out_entity_id,
                         WComponentTypeId & out_component_id,
                         WSubIdxId & out_subidx_id) const {

            IdType idcpy = id_;

            WAssetId::IdType asset_id = WAssetId(WID_NULL_V).GetId();
            WEntityId::IdType entity_id = WEntityId(WID_NULL_V).GetId();
            WComponentTypeId::IdType component_id = WComponentTypeId(WID_NULL_V).GetId();
            WSubIdxId::IdType subidx_id = WSubIdxId(WID_NULL_V).GetId();

            subidx_id &= ~WEntityComponentId_Meta::BitMaskV<WSubIdxId>;
            subidx_id |= WEntityComponentId_Meta::BitMaskV<WSubIdxId> & idcpy;
            idcpy >>= WEntityComponentId_Meta::BitsSizeV<WSubIdxId>;

            component_id &= ~WEntityComponentId_Meta::BitMaskV<WComponentTypeId>;
            component_id |= WEntityComponentId_Meta::BitMaskV<WComponentTypeId> & idcpy;
            idcpy >>= WEntityComponentId_Meta::BitsSizeV<WComponentTypeId>;

            entity_id &= ~WEntityComponentId_Meta::BitMaskV<WEntityId>;
            entity_id |= WEntityComponentId_Meta::BitMaskV<WEntityId> & idcpy;
            idcpy >>= WEntityComponentId_Meta::BitsSizeV<WEntityId>;

            asset_id &= ~WEntityComponentId_Meta::BitMaskV<WAssetId>;
            asset_id |= WEntityComponentId_Meta::BitMaskV<WAssetId> & idcpy;

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
    concept WAssetIndexId_Subtype =
        std::is_same_v<T, WAssetTypeId> ||
        std::is_same_v<T, WAssetId> ||
        std::is_same_v<T, WSubIdxId>;

    struct WTypeAssetIndexId_Meta{
        static constexpr std::uint8_t ASSET_TYPE_BITS_SIZE{16};
        static constexpr std::uint8_t ASSET_BITS_SIZE{32};
        static constexpr std::uint8_t SUBINDEX_BITS_SIZE{5};

        static constexpr std::uint8_t WID_BITS_SIZE{
            ASSET_TYPE_BITS_SIZE + ASSET_BITS_SIZE + SUBINDEX_BITS_SIZE};

        static_assert(WID_BITS_SIZE <= WID_COMPOUND_BITS_MAX);

        static constexpr std::uint64_t NULL_VALUE = ComputeCompoundNull<
            SWidSize<WAssetTypeId, ASSET_TYPE_BITS_SIZE>,
            SWidSize<WAssetId, ASSET_TYPE_BITS_SIZE>,
            SWidSize<WSubIdxId, SUBINDEX_BITS_SIZE>>();


        template<WAssetIndexId_Subtype T>
        static constexpr std::uint8_t GetBitsSize() {
            if constexpr(std::is_same_v<T, WAssetTypeId>) {
                return WTypeAssetIndexId_Meta::ASSET_TYPE_BITS_SIZE;
            }
            else if constexpr(std::is_same_v<T, WAssetId>) {
                return WTypeAssetIndexId_Meta::ASSET_BITS_SIZE;
            }
            else {
                return WTypeAssetIndexId_Meta::SUBINDEX_BITS_SIZE;
            }
        }

        template<WAssetIndexId_Subtype T>
        static constexpr std::uint8_t BitsSizeV = GetBitsSize<T>();

        template<WAssetIndexId_Subtype T>
        static constexpr std::size_t BitMaskV = GenBitMask(BitsSizeV<T>);

        static constexpr void ValidateWIds(WAssetTypeId asset_type_id,
                                           WAssetId asset_id,
                                           WSubIdxId subidx_id) {
            _ValidateWIds_<
                BitsSizeV<WAssetTypeId>,
                BitsSizeV<WAssetId>,
                BitsSizeV<WSubIdxId>>
                (
                    asset_type_id,
                    asset_id,
                    subidx_id
                    );
        }
    
    };

    /**
     * 
     */
    class WCORE_API WTypeAssetIndexId :
        public WId<std::uint64_t,
                   WTypeAssetIndexId_Meta,
                   WTypeAssetIndexId_Meta::NULL_VALUE> {
    public:

        using WId::WId;

        constexpr WTypeAssetIndexId() noexcept = default;
        constexpr WTypeAssetIndexId(WTypeAssetIndexId const &) noexcept = default;
        constexpr WTypeAssetIndexId(WTypeAssetIndexId&&) noexcept = default;
        constexpr WTypeAssetIndexId& operator=(WTypeAssetIndexId const &) noexcept = default;
        constexpr WTypeAssetIndexId& operator=(WTypeAssetIndexId&&) noexcept = default;
        ~WTypeAssetIndexId() = default;

        WTypeAssetIndexId(WAssetTypeId in_asset_type_id,
                          WAssetId in_asset_id,
                          WSubIdxId in_subidx) {

            WTypeAssetIndexId_Meta::ValidateWIds(
                in_asset_type_id, in_asset_id, in_subidx
                );

            id_ = 0;

            id_ |= WTypeAssetIndexId_Meta::BitMaskV<WAssetTypeId> & in_asset_type_id.GetId();

            id_ <<= WTypeAssetIndexId_Meta::BitsSizeV<WAssetId>;
            id_ |= WTypeAssetIndexId_Meta::BitMaskV<WAssetId> & in_asset_id.GetId();

            id_ <<= WTypeAssetIndexId_Meta::BitsSizeV<WSubIdxId>;
            id_ |= WTypeAssetIndexId_Meta::BitMaskV<WSubIdxId> & in_asset_id.GetId();
        }

        void ExtractWIds(WAssetTypeId & out_asset_type_id,
                         WAssetId & out_asset_id,
                         WSubIdxId & out_subidx) const {
            IdType idcpy = id_;

            WAssetTypeId::IdType asset_type_id = WAssetTypeId(WID_NULL_V).GetId();
            WAssetId::IdType asset_id = WAssetId(WID_NULL_V).GetId();
            WSubIdxId::IdType subidx = WSubIdxId(WID_NULL_V).GetId();

            subidx &= ~WTypeAssetIndexId_Meta::BitMaskV<WSubIdxId>;
            subidx |= WTypeAssetIndexId_Meta::BitMaskV<WSubIdxId> & idcpy;
            idcpy >>= WTypeAssetIndexId_Meta::BitsSizeV<WSubIdxId>;

            asset_id &= ~WTypeAssetIndexId_Meta::BitMaskV<WAssetId>;
            asset_id |= WTypeAssetIndexId_Meta::BitMaskV<WAssetId> & idcpy;
            idcpy >>= WTypeAssetIndexId_Meta::BitsSizeV<WAssetId>;

            asset_type_id &= ~WTypeAssetIndexId_Meta::BitMaskV<WAssetTypeId>;
            asset_type_id |= WTypeAssetIndexId_Meta::BitMaskV<WAssetTypeId> & idcpy; 

            out_asset_type_id = asset_type_id;
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

    struct WLevelSystemId_Meta{
        static constexpr std::uint8_t LEVEL_BITS_SIZE{
            WEntityComponentId_Meta::BitsSizeV<WAssetId>
        };
        
        static constexpr std::uint8_t SYSTEM_BITS_SIZE{16};

        static constexpr std::uint8_t WID_BITS_SIZE{LEVEL_BITS_SIZE + SYSTEM_BITS_SIZE};

        static_assert(WID_BITS_SIZE < WID_COMPOUND_BITS_MAX);

        template<WLevelSystemId_Subtype T>
        static constexpr std::uint8_t GetBitsSize() {
            if constexpr(std::is_same_v<T,WAssetId>) {
                return WLevelSystemId_Meta::LEVEL_BITS_SIZE;
            }
            else {
                return WLevelSystemId_Meta::SYSTEM_BITS_SIZE;
            }
        } 

        template<WLevelSystemId_Subtype T>
        static constexpr std::uint8_t BitsSizeV = GetBitsSize<T>();

        template<WLevelSystemId_Subtype T>
        static constexpr std::size_t BitMaskV = GenBitMask(BitsSizeV<T>);

        static constexpr std::uint64_t NULL_VALUE = ComputeCompoundNull<
            SWidSize<WAssetId, LEVEL_BITS_SIZE>,
            SWidSize<WSystemId, SYSTEM_BITS_SIZE>>();

        static constexpr void ValidateWIds(WAssetId asset_id, WSystemId system_id) {
            _ValidateWIds_<
                BitsSizeV<WAssetId>,
                BitsSizeV<WSystemId>
                > (
                    asset_id, system_id
                    );
        }
    };

    class WLevelSystemId :
        public WId<std::uint64_t,
                   WLevelSystemId_Meta,
                   WLevelSystemId_Meta::NULL_VALUE>{
    public:

        using WId::WId;

        constexpr WLevelSystemId() noexcept = default;
        constexpr WLevelSystemId(WLevelSystemId const &) noexcept = default;
        constexpr WLevelSystemId(WLevelSystemId &&) noexcept = default;
        constexpr WLevelSystemId& operator=(WLevelSystemId const &) noexcept = default;
        constexpr WLevelSystemId& operator=(WLevelSystemId &&) noexcept = default;
        ~WLevelSystemId() = default;

        WLevelSystemId(WAssetId in_asset_id, WSystemId in_system_id) {
            WLevelSystemId_Meta::ValidateWIds(in_asset_id, in_system_id);

            id_ = 0;

            id_ |= WLevelSystemId_Meta::BitMaskV<WAssetId> & in_asset_id.GetId();

            id_ <<= WLevelSystemId_Meta::BitsSizeV<WSystemId>;
            id_ |= WLevelSystemId_Meta::BitMaskV<WSystemId> & in_system_id.GetId();
        }

        void ExtractWIds(WAssetId out_asset_id, WSystemId out_system_id) const {
            IdType idcpy = id_;

            WAssetId::IdType asset_id=WAssetId(WID_NULL_V).GetId();
            WSystemId::IdType system_id=WSystemId(WID_NULL_V).GetId();

            system_id &= ~WLevelSystemId_Meta::BitMaskV<WSystemId>;
            system_id |= WLevelSystemId_Meta::BitMaskV<WSystemId> & idcpy;
            idcpy >>= WLevelSystemId_Meta::BitsSizeV<WSystemId>;

            asset_id &= ~WLevelSystemId_Meta::BitMaskV<WAssetId>;
            asset_id |= WLevelSystemId_Meta::BitMaskV<WAssetId> & idcpy;

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
        // 0b00 is free
    };

    /**
     * WEngId General id class
     */
    class WCORE_API WEngId {
    public:

    using IdType = std::uint64_t;

    private:

    // TODO null value deduced from compund types
    static constexpr std::uint64_t NullValue {0ULL};
    
    static constexpr std::uint64_t KindShift = WID_COMPOUND_BITS_MAX;
    static constexpr std::uint64_t KindMask  = std::numeric_limits<IdType>::max() << KindShift;

    static inline constexpr std::uint64_t GetKindBits(EObjectKind in_kind) noexcept {
        return static_cast<IdType>(in_kind) << KindShift;
    }

    public:

    constexpr WEngId() = default;
    constexpr WEngId(const WEngId&) = default;
    constexpr WEngId(WEngId&&) = default;
    constexpr WEngId& operator=(const WEngId&) = default;
    constexpr WEngId& operator=(WEngId&&) = default;
    ~WEngId() = default;

    constexpr WEngId (IdType in_id) noexcept : id_data_(in_id) {}

    constexpr WEngId(_WID_NULL_T_) noexcept : id_data_(NullValue) {}

    static constexpr WEngId FromAsset(WTypeAssetIndexId in_asset_index) noexcept {
        IdType payload = in_asset_index.GetId();
        return WEngId( GetKindBits(EObjectKind::Asset) | payload );
    }

    static constexpr WEngId FromEntityComponent(WEntityComponentId in_entt_component_id) noexcept {
        std::uint64_t payload = in_entt_component_id.GetId();
        return WEngId(GetKindBits(EObjectKind::EntityComponent) | payload );
    }

    static constexpr WEngId FromLevelSystem(WLevelSystemId in_level_system) noexcept {
        std::uint64_t payload = in_level_system.GetId();
        return WEngId(GetKindBits(EObjectKind::System) | payload);
    }

    constexpr bool IsValid() const noexcept { return id_data_ != NullValue; }

    constexpr EObjectKind Kind() const noexcept {
        return static_cast<EObjectKind>( (id_data_ & KindMask) >> KindShift );
    }

    constexpr WTypeAssetIndexId AsAssetIndexId() const noexcept {
        assert(Kind() == EObjectKind::Asset);
        return { id_data_ & ~KindMask };
    }

    constexpr WEntityComponentId AsEntityComponentId() const noexcept {
        assert(Kind() == EObjectKind::EntityComponent);
        return { id_data_ & ~KindMask };
    }

    constexpr WLevelSystemId AsLevelSystemId() const noexcept {
        assert(Kind() == EObjectKind::System);
        return { id_data_ & ~KindMask };
    }

    constexpr explicit operator std::uint64_t() const noexcept { return id_data_; }

    constexpr bool operator==(const WEngId& o) const noexcept { return id_data_ == o.id_data_; }
    constexpr bool operator!=(const WEngId& o) const noexcept { return id_data_ != o.id_data_; }
    constexpr bool operator<(const WEngId& o) const noexcept { return id_data_ < o.id_data_; }

    struct Hash {
        std::size_t operator()(const WEngId& id) const noexcept {
            return std::hash<std::uint64_t>{}(id.id_data_);
        }
    };

    private:

    std::uint64_t id_data_{NullValue};

    };


}

namespace std
{
    template <wid::CIsWId T>
    struct hash<T>
    {
        std::size_t operator()(T const & in_wid) const
            {
                return std::hash<typename T::IdType>{}(in_wid.GetId());
            }
    };
}


namespace std {
    template<>
    struct hash<wid::WEngId> {
        std::size_t operator()(const wid::WEngId& id) const noexcept {
            return wid::WEngId::Hash{}(id);
        }
    };
}
