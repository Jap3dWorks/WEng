#pragma once

#include "WCoreTypes/WRenderTypes.hpp"
#include "WCore/TSparseSet.hpp"

// #include <unordered_map>
#include <cstdint>
#include <span>

namespace wrd::lighting {

    template<typename T, std::uint32_t Size=0>
    class StaticSpanAllocator {
    public:

        using value_type = T;
        using pointer = T*;
        using size_type = std::uint32_t;

    public:

        // StaticSpanAllocator() = default;
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
            return std::min(mem_.size, Size);
        }

        bool operator==(const StaticSpanAllocator& other) const {
            return mem_ == other.mem_;
        }

        bool operator!=(const StaticSpanAllocator& other) const {
            return mem_ != other.mem_;
        }

    private:

        std::span<T, Size> mem_{nullptr};

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
        DenseStaticMemController(DenseStaticMemController&&) = default;
        DenseStaticMemController& operator=(const DenseStaticMemController&) = default;
        DenseStaticMemController& operator=(DenseStaticMemController&&) = default;
        virtual ~DenseStaticMemController() = default;

        DenseStaticMemController(std::array<T,MAX> & light_data) :
            index_array_(),
            light_set_(
                Alloc<T>(light_data),
                Alloc<std::size_t>(index_array_)
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

        std::uint32_t Size() const {
            return light_set_.Count();
        }

    private:

        LightSet light_set_{};
        IndexArray index_array_{};

    };

    /**
     * 
     */
    class WLightingUBOController {
    public:

        WLightingUBOController() :
            lighting_ubo_(),
            pl_controller_(lighting_ubo_.point_lights),
            dl_controller_(lighting_ubo_.directional_lights) {}

        WLightingUBOController(const WLightingUBOController&) = default;
        WLightingUBOController(WLightingUBOController&&) = default;
        WLightingUBOController& operator=(const WLightingUBOController&) = default;
        WLightingUBOController& operator=(WLightingUBOController&&) = default;
        virtual ~WLightingUBOController() = default;

        void UpdatePointLight(WEntityComponentId in_component_id,
                              const wct::render::WPointLight & in_light) {
            pl_controller_.Insert(in_component_id, in_light);
            lighting_ubo_.point_lights_count = pl_controller_.Size();
        }

        void RemovePointLight(WEntityComponentId in_component_id) {
            pl_controller_.Remove(in_component_id);
            lighting_ubo_.point_lights_count = pl_controller_.Size();
        }

        void UpdateDirectionalLight(WEntityComponentId in_component_id,
                                    const wct::render::WDirectionalLight & in_light) {
            dl_controller_.Insert(in_component_id, in_light);
            lighting_ubo_.directional_lights_count = dl_controller_.Size();
        }

        void RemoveDirectionalLight(WEntityComponentId in_component_id) {
            dl_controller_.Remove(in_component_id);
            lighting_ubo_.directional_lights_count = dl_controller_.Size();
        }

        void UpdateAmbientLight(const wct::render::WAmbientLight & in_light) {
            lighting_ubo_.ambient_light = in_light;
        }

        const wct::render::WLightingUBO & LightingUbo() const {
            return lighting_ubo_;
        }

    private:

        wct::render::WLightingUBO lighting_ubo_{};

        DenseStaticMemController<
            wct::render::WPointLight,
            wct::render::WLightingUBO::MAX_POINT_LIGHTS,
            StaticSpanAllocator> pl_controller_;

        DenseStaticMemController<
            wct::render::WDirectionalLight,
            wct::render::WLightingUBO::MAX_DIRECTIONAL_LIGHTS,
            StaticSpanAllocator> dl_controller_;
    };

}
