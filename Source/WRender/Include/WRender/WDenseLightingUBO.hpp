#pragma once

#include "WCoreTypes/WRenderTypes.hpp"
#include "WCore/TSparseSet.hpp"
#include "WLog.hpp"

#include <cstdint>
#include <limits>
#include <span>

namespace wrd::lighting {

    template<typename T, std::uint32_t Size=0>
    class StaticSpanAllocator {
    public:

        using value_type = T;
        using pointer = T*;
        using size_type = std::uint32_t;

    public:

        StaticSpanAllocator(const StaticSpanAllocator&) = default;
        StaticSpanAllocator(StaticSpanAllocator&&) = default;
        StaticSpanAllocator& operator=(const StaticSpanAllocator&) = default;
        StaticSpanAllocator& operator=(StaticSpanAllocator&&) = default;
        virtual ~StaticSpanAllocator() = default;

        StaticSpanAllocator(std::span<T,Size> value) :
            mem_(value) {}

    public:

        WNODISCARD constexpr pointer allocate(size_type n) noexcept {
            return mem_.empty() ? nullptr : mem_.data();
        }

        void deallocate(pointer p, size_type n) noexcept {}

        size_type max_size() const {
            return Size;
        }

        bool operator==(const StaticSpanAllocator& other) const {
            return mem_ == other.mem_;
        }

        bool operator!=(const StaticSpanAllocator& other) const {
            return mem_ != other.mem_;
        }

    private:

        std::span<T, Size> mem_;

    };

    template<typename T,
             std::uint32_t MAX,
             template <typename, std::uint32_t> typename TStaticAlloc=StaticSpanAllocator>
    struct DenseStaticMemController{
    private:

        /**
         * This class is required to fix an std library error with alloc_traits.h
         * when allocator has more than 1 templated parameter.
         */
        template<typename D>
        class Alloc : public TStaticAlloc<D, MAX> {
        public:
            using TStaticAlloc<D, MAX>::TStaticAlloc;
        };
            
    public:

        using IndexArray = std::array<std::size_t, MAX>;

        using LightSet = TSparseSet<T,
                                    Alloc<T>,
                                    Alloc<std::size_t>>;

    public:

        DenseStaticMemController() = default;
        DenseStaticMemController(const DenseStaticMemController&) = default;
        DenseStaticMemController& operator=(const DenseStaticMemController&) = default;
        DenseStaticMemController(DenseStaticMemController&&) = default;
        DenseStaticMemController& operator=(DenseStaticMemController&&) = default;
        virtual ~DenseStaticMemController() = default;

        DenseStaticMemController(std::array<T,MAX> & light_mem) :
            _index_mem_(),
            light_set_(
                Alloc<T>(light_mem),
                Alloc<std::size_t>(_index_mem_)
                ) {}

        void Insert(std::size_t in_id, const T & in_value) {
            light_set_.Insert(in_id, in_value);
        }

        void Remove(std::size_t in_id) {
            light_set_.Remove(in_id);
        }

        void Update(std::size_t in_id,
                    wct::render::WPointLight point_light) {
            light_set_.Insert(in_id, point_light);
        }

        bool Contains(std::size_t in_id) const {
            return light_set_.Contains(in_id);
        }

        std::uint32_t Count() const {
            return light_set_.Count();
        }

        void Clear() {
            light_set_.Clear();
        }

        WNODISCARD std::uint32_t DensePosition(std::size_t in_id) const {
            return light_set_.DensePosition(in_id);
        }

    private:

        LightSet light_set_{};

        IndexArray _index_mem_{};

    };

    template<typename T, std::uint32_t MaxLights, typename CountType>
    struct WLightDenseController {

        WLightDenseController() = default;
        WLightDenseController(const WLightDenseController&) = delete;
        WLightDenseController& operator=(const WLightDenseController&) = delete;

        WLightDenseController(WLightDenseController&&) = default;
        WLightDenseController& operator=(WLightDenseController&&) = default;
        virtual ~WLightDenseController() = default;

        WLightDenseController(
            std::array<T, MaxLights> & in_lights_data_ref,
            CountType & in_count_ref
            ) :
            count_ref_(&in_count_ref),
            controller_(in_lights_data_ref)
            {}


        void Update(WEntityComponentId in_component_id,
                              const T & in_light) {

            controller_.Insert(in_component_id, in_light);
            *count_ref_ = controller_.Count();

            #ifndef NDEBUG
            if (MaxLights == *count_ref_) {
                WFLOG("Max lights achieved: MAX {}, Lights {}",
                      MaxLights,
                      *count_ref_);
            }
            #endif
        }

        void Update(std::span<WEntityComponentId> in_ids,
                               std::span<T> in_point_lights) {
            for(std::uint32_t i=0; i<in_ids.size(); i++) {
                Update(
                    in_ids[i],
                    in_point_lights[i]
                    );
            }
        }

        void RemoveLight(WEntityComponentId in_component_id) {
            controller_.Remove(in_component_id);
            *count_ref_ = controller_.Count();
        }

        void Clear() {
            controller_.Clear();
            *count_ref_ = controller_.Count();
        }


        WNODISCARD bool Contains(WEntityComponentId in_id) const {
            return controller_.Contains(in_id);
        }

        WNODISCARD std::uint32_t Count() const {
            return controller_.Count();
        }

        WNODISCARD std::uint32_t DensePosition(WEntityComponentId in_id) const {
            return controller_.DensePosition(in_id);
        }

        WNODISCARD std::uint32_t FirstDensePosition(std::span<WEntityComponentId> in_ids) const {
            assert(!in_ids.empty());

            std::uint32_t lower = std::numeric_limits<std::uint32_t>::max();

            for (auto id : in_ids) {
                std::uint32_t tmp = DensePosition(id);
                if (tmp < lower) {
                    lower = tmp;
                }
            }

            return std::min(lower, *count_ref_);
            return lower;
        }

    private:

        CountType * count_ref_{nullptr};

        DenseStaticMemController<
            T,
            MaxLights,
            StaticSpanAllocator> controller_;
        
    };

    /**
     * LightingUBO with dense data structure.
     * This class ensures that light data is dense and located in the initial
     * positions of each light type array in WLightingUBO.
     * This class owns the WLightingUBO.
     */
    class WDenseLightingUBO {
    public:

        using  PointLightDC = WLightDenseController<
            wct::render::WPointLight,
            wct::render::WLightingUBO::MAX_POINT_LIGHTS,
            decltype(wct::render::WLightingUBO::point_lights_count)
            >;

        using DirectionalLightDC = WLightDenseController<
            wct::render::WDirectionalLight,
            wct::render::WLightingUBO::MAX_DIRECTIONAL_LIGHTS,
            decltype(wct::render::WLightingUBO::directional_lights_count)
            >;

        WDenseLightingUBO() :
            lighting_ubo_() {}

        WDenseLightingUBO(const WDenseLightingUBO&) = delete;
        WDenseLightingUBO& operator=(const WDenseLightingUBO&) = delete;

        WDenseLightingUBO(WDenseLightingUBO&&) = default;
        WDenseLightingUBO& operator=(WDenseLightingUBO&&) = default;
        virtual ~WDenseLightingUBO() = default;

        void Clear() {
            PointLightDenseController().Clear();
            DirectionalLightDenseController().Clear();
            lighting_ubo_.ambient_light = {};
        }

        WNODISCARD PointLightDC PointLightDenseController() {
            return {lighting_ubo_.point_lights, lighting_ubo_.point_lights_count};
        }

        WNODISCARD DirectionalLightDC DirectionalLightDenseController() {
            return {lighting_ubo_.directional_lights, lighting_ubo_.directional_lights_count};
        }

        void UpdateAmbientLight(const wct::render::WAmbientLight & in_light) {
            lighting_ubo_.ambient_light = in_light;
        }

        const wct::render::WLightingUBO & LightingUbo() const {
            return lighting_ubo_;
        }

    private:

        wct::render::WLightingUBO lighting_ubo_{};
        
    };
}
