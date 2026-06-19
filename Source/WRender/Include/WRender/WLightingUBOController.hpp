#pragma once

#include "WCoreTypes/WRenderTypes.hpp"

#include <unordered_map>
#include <cstdint>
#include <span>

namespace wrd::lighting {


    template<std::uint32_t N, typename T>
    class StaticSpanAllocator {

    public:

        StaticSpanAllocator() = default;
        StaticSpanAllocator(const StaticSpanAllocator&) = default;
        StaticSpanAllocator(StaticSpanAllocator&&) = default;
        StaticSpanAllocator& operator=(const StaticSpanAllocator&) = default;
        StaticSpanAllocator& operator=(StaticSpanAllocator&&) = default;
        virtual ~StaticSpanAllocator() = default;

        StaticSpanAllocator(std::span<T,N> value) :
            mem_(value) {}

        using pointer = T*;
        using value_type = T;
        using size_type = std::uint32_t;

    public:

        pointer allocate(size_type n) {
            return mem_.data();
        }

        void deallocate(pointer p, size_type n) {
            
        }

        constexpr size_type max_size() const {
            return N;
        }

        bool operator==(const StaticSpanAllocator& other) const {
            return mem_ == other.mem_;
        }

        bool operator!=(const StaticSpanAllocator& other) const {
            return mem_ != other.mem_;
        }

    private:

        std::span<T, N> mem_;

    };

    // template<std::uint32_t N, typename T>
    // class UboLightTrack {
    // public:
        
    //     UboLightTrack(std::span<T> in_data) {
    //         light_map.reserve(N), light_data(in_data);
    //     }

    //     UboLightTrack(const UboLightTrack&) = default;
    //     UboLightTrack(UboLightTrack&&) = default;
    //     UboLightTrack& operator=(const UboLightTrack&) = default;
    //     UboLightTrack& operator=(UboLightTrack&&) = default;
    //     virtual ~UboLightTrack() = default;

    //     void PushBack(WEntityComponentId in_id, T data) {
    //         light_data[count_]=data;

    //         light_map[in_id]=count_;
    //         light_stack[count_]=in_id;

    //         count_++;
    //     }

    //     std::tuple<WEntityComponentId, T> Back() const {
    //         WEntityComponentId cmpnt = light_stack[count_-1];
    //         std::uint32_t indx = light_map.at(light_stack[count_-1]);
            
    //         return {cmpnt, light_data[indx]};
    //     }

    //     void Erase(WEntityComponentId in_id) {
    //         WEntityComponentId lstcmp = light_stack[count_-1];
    //         std::uint32_t lstindx = light_map[lstcmp];

    //         std::uint32_t rmvindx = light_map[in_id];
    //         light_map.erase(in_id);

    //         light_data[rmvindx] = light_data[lstindx];
    //         light_map[lstcmp] = rmvindx;

    //         light_stack[rmvindx] = lstcmp;

    //         PopBack();
    //     }

    //     void PopBack() {
    //         light_map.erase(light_stack[count_-1]);
    //         count_--;
    //     }

    //     std::uint32_t Get(WEntityComponentId in_id) const {
    //         return light_map.at(in_id);
    //     }

    //     std::uint32_t Count() const {
    //         return count_;
    //     }

    // private:
        
    //     std::unordered_map<WEntityComponentId, std::uint32_t> light_map{};
    //     std::array<WEntityComponentId, N> light_stack{};
    //     std::span<T> light_data;

    //     std::uint32_t count_{0};
    // };

    class WLightingUBOController {
    public:

        WLightingUBOController() = default;
        WLightingUBOController(const WLightingUBOController&) = default;
        WLightingUBOController(WLightingUBOController&&) = default;
        WLightingUBOController& operator=(const WLightingUBOController&) = default;
        WLightingUBOController& operator=(WLightingUBOController&&) = default;
        virtual ~WLightingUBOController() = default;

        void AddPointLight(WEntityComponentId in_component_id,
                           wct::render::WPointLight point_light) {
            std::uint32_t count = point_track.Count();

            lighting_ubo_.point_lights[count]= point_light;

            point_track.PushBack(in_component_id);

            lighting_ubo_.point_lights_count = point_track.Count();
        }

        void RemovePointLight(WEntityComponentId in_component_id) {
            std::uint32_t rmv_indx = point_light_map[in_component_id];

            lighting_ubo_.point_lights[rmv_indx] =
                lighting_ubo_.point_lights[lighting_ubo_.point_lights_count - 1];

            point_light_map[point_light_stack.back()] = rmv_indx;

            point_light_stack[rmv_indx] = point_light_stack.back();
            point_light_stack.pop_back();

            std::array<WEntityComponentId, 32> stack;

        }

        void UpdatePointLight(WEntityComponentId in_component_id,
                              wct::render::WPointLight point_light) {
            std::uint32_t indx = point_light_map[in_component_id];
            lighting_ubo_.point_lights[indx] = point_light;
        }

    private:

        wct::render::WLightingUBO lighting_ubo_{};

        UboLightTrack<wct::render::WLightingUBO::MAX_POINT_LIGHTS> point_track{};

        // std::unordered_map<WEntityComponentId, std::uint32_t> point_light_map{};
        // std::vector<WEntityComponentId> point_light_stack{};
    };

}
