#pragma once

#include "WCoreTypes/WRenderTypes.hpp"
#include "WCore/TSparseSet.hpp"
#include "WLog.hpp"
#include "WRender/WLight.hpp"

#include <cstdint>
#include <limits>
#include <span>

namespace wrd::light {

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

        DenseStaticMemController(std::array<T,MAX> & light_mem, std::size_t shadow_casters) :
            _index_mem_(),
            light_set_(
                Alloc<T>(light_mem),
                Alloc<std::size_t>(_index_mem_)
                ),
            shadow_casters_(shadow_casters) {}

        // TODO is shadow caster?
        void Insert(std::size_t in_id, const T & in_value) {
            light_set_.Insert(in_id, in_value);
        }

        void Remove(std::size_t in_id) {

            auto pos = light_set_.DensePosition(in_id);

            if (shadow_casters_ > 0 && pos < shadow_casters_-1) {
                auto last_caster_id = light_set_.IndexInDensePosition(shadow_casters_-1);
                light_set_.SwapDensePositions(in_id, last_caster_id);
                --shadow_casters_;
            }

            light_set_.Remove(in_id);
        }

        bool Contains(std::size_t in_id) const {
            return light_set_.Contains(in_id);
        }

        std::uint32_t Count() const {
            return light_set_.Count();
        }

        std::uint32_t ShadowCasters() const {
            return shadow_casters_;
        }

        void Clear() {
            light_set_.Clear();
            shadow_casters_=0;
        }

        WNODISCARD std::uint32_t DensePosition(std::size_t in_id) const {
            return light_set_.IndexInDensePosition(in_id);
        }

        WNODISCARD T const * DenseData() const {
            return light_set_.DenseData();
        }

    private:

        LightSet light_set_{};

        IndexArray _index_mem_{};  // real memory to allocate the index array

        std::uint32_t shadow_casters_{0};

    };

    template<typename T, std::uint32_t MaxLights, typename CountType>
    struct WLightDenseController {

        WLightDenseController() = default;
        WLightDenseController(WLightDenseController const &) = delete;
        WLightDenseController& operator=(WLightDenseController const &) = delete;

        WLightDenseController(WLightDenseController&&) = default;
        WLightDenseController& operator=(WLightDenseController&&) = default;
        virtual ~WLightDenseController() = default;

        WLightDenseController(
            std::array<T, MaxLights> & in_lights_data_ref,
            CountType & count_ref,
            std::uint32_t & shadow_casters_ref
            ) :
            count_ref_(&count_ref),
            shadow_casters_(&shadow_casters_ref),
            controller_(in_lights_data_ref, shadow_casters_ref) // TODO: pass index array too
            {}

        void Update(wcr::wid::WEntityComponentId in_component_id,
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

        void Update(std::span<wcr::wid::WEntityComponentId> in_ids,
                    std::span<T> in_point_lights) {
            for(std::uint32_t i=0; i<in_ids.size(); i++) {
                Update(
                    in_ids[i],
                    in_point_lights[i]
                    );
            }
        }

        void RemoveLight(wcr::wid::WEntityComponentId in_component_id) {
            controller_.Remove(in_component_id);
            *count_ref_ = controller_.Count();
            *shadow_casters_ = controller_.ShadowCasters();
        }

        void Clear() {
            controller_.Clear();
            *count_ref_ = controller_.Count();
            *shadow_casters_=controller_.ShadowCasters();
        }


        WNODISCARD bool Contains(wcr::wid::WEntityComponentId in_id) const {
            return controller_.Contains(in_id);
        }

        WNODISCARD std::uint32_t Count() const {
            return controller_.Count();
        }

        WNODISCARD std::uint32_t DensePosition(wcr::wid::WEntityComponentId in_id) const {
            return controller_.DensePosition(in_id);
        }

        WNODISCARD T const * DenseData() const {
            return controller_.DenseData();
        }

        /**
         * @returns {<first>, <last>}
         * <first> is the lower dense position. <last> is the higher dense position.
         */
        WNODISCARD std::tuple<std::uint32_t, std::uint32_t>
        FirstLastDensePosition(
            std::span<wcr::wid::WEntityComponentId> in_ids
            ) const {
            assert(!in_ids.empty());

            std::uint32_t first = std::numeric_limits<std::uint32_t>::max();
            std::uint32_t last = 0;

            for (auto id : in_ids) {
                std::uint32_t tmp = DensePosition(id);
                if (tmp < first) {
                    first = tmp;
                }
                if (tmp > last) {
                    last = tmp;
                }
            }

            return {std::min(first, *count_ref_), last};
        }

    private:

        CountType * count_ref_{nullptr};
        std::uint32_t * shadow_casters_{nullptr};

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
            wct::render::PointLight,
            wct::render::LightingUBO::MAX_POINT_LIGHTS,
            decltype(wct::render::LightingUBO::point_lights_count)
            >;

        using DirectionalLightDC = WLightDenseController<
            wct::render::DirectionalLight,
            wct::render::LightingUBO::MAX_DIRECTIONAL_LIGHTS,
            decltype(wct::render::LightingUBO::directional_lights_count)
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
            return {lighting_ubo_.point_lights,
                    lighting_ubo_.point_lights_count,
                    tmp_pl_shadow_cast_};
        }

        WNODISCARD DirectionalLightDC DirectionalLightDenseController() {
            return {lighting_ubo_.directional_lights,
                    lighting_ubo_.directional_lights_count,
                    lighting_ubo_.directional_shadow_casters};
        }

        void UpdateAmbientLight(const wct::render::AmbientLight & in_light) {
            lighting_ubo_.ambient_light = in_light;
        }

        void SetDirectionalLightShadowCasters(std::uint32_t shadow_casters) {
            lighting_ubo_.directional_shadow_casters=shadow_casters;
        }

        const wct::render::LightingUBO & LightingUbo() const {
            return lighting_ubo_;
        }

        void UpdateShadowMap(
            glm::mat4 projection_matrix,
            glm::mat4 view_matrix
            ) {
            lighting_ubo_.shadow_map_projection = projection_matrix;
            lighting_ubo_.shadow_map_view_matrix = view_matrix;
        }

    private:

        wct::render::LightingUBO lighting_ubo_{};

        // TODO light index arrays

        std::uint32_t tmp_pl_shadow_cast_;
    };
}
