#pragma once

#include "WCore/TypeTraits.hpp"
#include "WComponents/Transform.hpp"
#include "WCore/WId.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WInterfaces/IRender.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WComponents/Light/Point.hpp"
#include "WComponents/Light/Directional.hpp"
#include "WComponents/Light/Ambient.hpp"
#include "WAssets/Level.hpp"
#include "WRender/WLight.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WEngRender/Camera.hpp"

#include <cstdint>
#include <span>

namespace wng::render::lights {

    struct ShadowMap {
        glm::mat4 projection{};
        glm::mat4 view{};

        static inline constexpr ShadowMap GetOrthoShadowMap() {
            ShadowMap result;
            result.projection = wrd::light::ToShadowMapProjectionMatrix(
                3.f, 3.f, -8.f, 25.f
                );

            return result;
        }
    };
    
    template<typename LightsArray>
    struct LightsRenderData {
        
        using ElementType = wcr::type_traits::ElementType_t<LightsArray>;

        LightsArray lights;
        std::array<
            wcr::wid::WEntityComponentId,
            wcr::type_traits::MaxSize_v<LightsArray>> ids;

        std::size_t count{0};

        void AddLight(ElementType light,
                      wcr::wid::WEntityComponentId id) {
            lights[count]=std::move(light);
            ids[count]=id;
            count++;
        }

        auto lights_begin() {
            return lights.begin();
        }

        auto lights_end() {
            return lights.begin() + count;
        }

        auto ids_begin() {
            return ids.begin();
        }

        auto ids_end() {
            return ids.begin() + count;
        }

        static constexpr auto Merge(LightsRenderData a, LightsRenderData b) {
            std::vector<wcr::wid::WEntityComponentId> merged_ids{
                a.ids_begin(), a.ids_end()};
            
            merged_ids.insert(
                merged_ids.end(),
                b.ids_begin(), b.ids_end()
                );

            std::vector<wct::render::DirectionalLight> merged_lights{
                a.lights_begin(), a.lights_end()
            };
            merged_lights.insert(
                merged_lights.end(),
                b.lights_begin(), b.lights_end()
                );

            return std::tuple{std::move(merged_ids), std::move(merged_lights)};
        }
    };

    inline void InitializeLights(
        IRender * in_render,
        was::Level * in_level,
        const WAssetDb & in_asset_db
        ) {

        // Cast shadows lights go first than no cast shadows lights
        // lighting shaders requires less computing with this organization.

        // Point Lights

        decltype(wct::render::LightingUBO::point_lights) point_lights;
        std::array<wcr::wid::WEntityComponentId, point_lights.size()> pl_ids;
        std::uint32_t pl_count=0;

        in_level->ForEachComponent<wcm::light::Point>(
            [&in_level, &point_lights, &pl_ids, &pl_count]
            (wcm::light::Point * cmp) {
                if (cmp->Get_active()) {

                    auto * transform_component =
                        &in_level->GetComponent<wcm::Transform>
                        (cmp->Get_entity_id());

                    auto plight = wrd::light::ToPointLight(
                        transform_component->Get_position(),
                        cmp->Get_radius(),
                        cmp->Get_color(),
                        cmp->Get_intensity()
                        );
                    
                    point_lights[pl_count] = plight;

                    pl_ids[pl_count] = {
                        in_level->Get_asset_id(),
                        cmp->Get_entity_id(),
                        in_level->GetComponentTypeId<wcm::light::Point>(),
                        wcr::wid::nullid
                    };

                    pl_count++;
                }
            }
            );

        // Directional Lights

        ShadowMap shadow_map_dt=ShadowMap::GetOrthoShadowMap();

        using DirectionalLightsArray = decltype(wct::render::LightingUBO::directional_lights);

        LightsRenderData<DirectionalLightsArray> directional_lights;
        // Current state only 1 shadow caster / shadow map
        LightsRenderData<DirectionalLightsArray> directional_shadow_caster;  

        in_level->ForEachComponent<wcm::light::Directional>(
            [&in_level,
             &directional_lights,
             &directional_shadow_caster,
             &shadow_map_dt
                ]
            (wcm::light::Directional * cmp) {
                if (cmp->Get_active()) {

                    auto * transform_cmp = &in_level
                        ->GetComponent<wcm::Transform>
                        (cmp->Get_entity_id());

                    auto dlight = wrd::light::ToDirectionalLight(
                        transform_cmp->Get_transform_matrix()[0],  // x direction
                        cmp->Get_color(),
                        cmp->Get_intensity()
                        );

                    dlight.direction = transform_cmp->Get_transform_matrix()[0];

                    if (cmp->Get_cast_shadows()) {

                        shadow_map_dt.view = wrd::light::ToShadowMapViewMatrix(
                            transform_cmp->Get_transform_matrix(),
                            {0.f, 0.f, 0.f}  // TODO parametrizable
                            );

                        directional_shadow_caster.AddLight(
                            dlight,
                            {
                                in_level->Get_asset_id(),
                                cmp->Get_entity_id(),
                                in_level->GetComponentTypeId<wcm::light::Directional>(),
                                wcr::wid::nullid
                            }   
                            );
                    }
                    else {
                        directional_lights.AddLight(
                            dlight,
                            {
                                in_level->Get_asset_id(),
                                cmp->Get_entity_id(),
                                in_level->GetComponentTypeId<wcm::light::Directional>(),
                                wcr::wid::nullid
                            }
                            );
                    }
                }
            }
            );

        // Ambient Light

        wct::render::AmbientLight amb_light;
        in_level->ForEachComponent<wcm::light::Ambient>(
            [&amb_light](auto * cmp) {
                amb_light = wrd::light::ToAmbientLight(*cmp);
            }
            );

        std::uint32_t dl_shdw = directional_shadow_caster.count;

        auto[dl_ids, dl_lights] = LightsRenderData<DirectionalLightsArray>::Merge(
            std::move(directional_shadow_caster),
            std::move(directional_lights)
            );

        in_render->InitializeLights(
            {pl_ids.begin(), pl_ids.begin() + pl_count},
            {point_lights.begin(), point_lights.begin() + pl_count},
            dl_ids,
            dl_lights,
            amb_light,
            dl_shdw
            );

        in_render->UpdateShadowMap(
            shadow_map_dt.projection,
            shadow_map_dt.view
            );

    }

    inline void UpdateShadowMap(
        IRender * render,
        was::Level * level
        ) {
        wcr::wid::WEntityId cam_id = wng::render::camera::GetRenderCamera(level);

        wcm::Camera & cam_cmp = level->GetComponent<wcm::Camera>(cam_id);
        wcm::Transform & trn_cmp = level->GetComponent<wcm::Transform>(cam_id);

        glm::vec3 point_of_interest = cam_cmp.GetPointOfInterest(
            trn_cmp.Get_position(),
            wcm::Camera::GetCameraFrontVector(trn_cmp.Get_transform_matrix())
            );

        ShadowMap shadow_map;

        wcm::light::Directional * dl_cmp{nullptr};

        level->ForEachComponent<wcm::light::Directional>(
            [&dl_cmp, &level, &render, &point_of_interest]
            (wcm::light::Directional * cmp) {
                if ((!dl_cmp) && cmp->Get_active() && cmp->Get_cast_shadows()) {
                    dl_cmp = cmp;
                    auto transform_cmp = level->GetComponent<wcm::Transform>(cmp->Get_entity_id());

                    ShadowMap shadow_map=ShadowMap::GetOrthoShadowMap();

                    shadow_map.view = wrd::light::ToShadowMapViewMatrix(
                        transform_cmp.Get_transform_matrix(),
                        point_of_interest
                        );

                    render->UpdateShadowMap(
                        shadow_map.projection,
                        shadow_map.view
                        );
                }
            }
            );

    }


}
