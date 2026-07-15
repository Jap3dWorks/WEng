#include "WCore/WCore.hpp"
#include "WImporter/WImporterGltf.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WAssets/Level.hpp"
#include "WComponents/Light/WDirectionalLightComponent.hpp"
#include "WComponents/Light/WPointLightComponent.hpp"
#include "WComponents/WStaticMeshComponent.hpp"
#include "WCore/WId.hpp"
#include "WCoreTypes/WGeometry.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WCoreTypes/WTexture.hpp"
#include "WEngineInterfaces/IRender.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WString/WString.hpp"
#include "fastgltf/core.hpp"
#include "fastgltf/types.hpp"
#include "fastgltf/tools.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WRenderPipelineParametersAsset.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WComponents/WCameraComponent.hpp"
#include "fastgltf/util.hpp"
#include "WLog.hpp"

#include <WLib_stbi.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/type_trait.hpp>

#include <limits>
#include <stdexcept>
#include <type_traits>

std::vector<std::string_view> wim::importer::WImporterGltf::Extensions() const noexcept {
    return {".gltf", ".glb"};
}

std::vector<std::string_view> wim::importer::WImporterGltf::Formats() const noexcept {
    return {"gltf"};
}

std::unique_ptr<wim::importer::WImporter> wim::importer::WImporterGltf::Clone() {
    return std::make_unique<WImporterGltf>(*this);
}

template <>
struct fastgltf::ElementTraits<glm::vec2> :
    fastgltf::ElementTraitsBase<glm::vec2, AccessorType::Vec2, float> {};

template <>
struct fastgltf::ElementTraits<glm::vec3> :
    fastgltf::ElementTraitsBase<glm::vec3, AccessorType::Vec3, float> {};

template <>
struct fastgltf::ElementTraits<glm::vec4> :
    fastgltf::ElementTraitsBase<glm::vec4, AccessorType::Vec4, float> {};

namespace {

    template<typename T=std::size_t>
    using NullableIndex = wid::WId<T,
                                   wid::WIdDefaultFlag,
                                   std::numeric_limits<T>::max()>;

    WNODISCARD constexpr wct::texture::ESampler MagFilter(fastgltf::Filter mag_filter) noexcept {
        switch(mag_filter) {
        case fastgltf::Filter::Linear:
            return wct::texture::ESampler::MAG_LINEAR;
        case fastgltf::Filter::LinearMipMapLinear:
            return wct::texture::ESampler::MAG_LINEAR;
        case fastgltf::Filter::LinearMipMapNearest:
            return wct::texture::ESampler::MAG_LINEAR;
        case fastgltf::Filter::Nearest:
            return wct::texture::ESampler::MAG_NEAREST;
        case fastgltf::Filter::NearestMipMapLinear:
            return wct::texture::ESampler::MAG_NEAREST;
        case fastgltf::Filter::NearestMipMapNearest:
            return wct::texture::ESampler::MAG_NEAREST;
        default:
            return wct::texture::ESampler::MAG_LINEAR;
        }
    }

    WNODISCARD constexpr wct::texture::ESampler MinFilter(fastgltf::Filter min_filter) noexcept {
        switch(min_filter) {
        case fastgltf::Filter::Linear:
            return wct::texture::ESampler::MIN_LINEAR;
        case fastgltf::Filter::LinearMipMapLinear:
            return wct::texture::ESampler::MIN_LINEAR;
        case fastgltf::Filter::LinearMipMapNearest:
            return wct::texture::ESampler::MIN_LINEAR;
        case fastgltf::Filter::Nearest:
            return wct::texture::ESampler::MIN_NEAREST;
        case fastgltf::Filter::NearestMipMapLinear:
            return wct::texture::ESampler::MIN_NEAREST;
        case fastgltf::Filter::NearestMipMapNearest:
            return wct::texture::ESampler::MIN_NEAREST;
        default:
            return wct::texture::ESampler::MIN_LINEAR;
        }
    }

    WNODISCARD constexpr wct::texture::ESampler WrapSFilter(fastgltf::Wrap wrap_filter) noexcept {
        switch(wrap_filter) {
        case fastgltf::Wrap::ClampToEdge:
            return wct::texture::ESampler::WRAPS_CLAMP;
        case fastgltf::Wrap::MirroredRepeat:
            return wct::texture::ESampler::WRAPS_REPEAT;
        case fastgltf::Wrap::Repeat:
            return wct::texture::ESampler::WRAPS_REPEAT;
        default:
            return wct::texture::ESampler::WRAPS_REPEAT;
        }
    }    

    WNODISCARD constexpr wct::texture::ESampler WrapTFilter(fastgltf::Wrap wrap_filter) noexcept {
        switch(wrap_filter) {
        case fastgltf::Wrap::ClampToEdge:
            return wct::texture::ESampler::WRAPT_CLAMP;
        case fastgltf::Wrap::MirroredRepeat:
            return wct::texture::ESampler::WRAPT_REPEAT;
        case fastgltf::Wrap::Repeat:
            return wct::texture::ESampler::WRAPT_REPEAT;
        default:
            return wct::texture::ESampler::WRAPT_REPEAT;
        }
    }    

    inline wct::texture::ESampler ToESampler(fastgltf::Sampler const & sampler) {
        return MinFilter(sampler.minFilter.value_or(fastgltf::Filter::Linear)) |
            MagFilter(sampler.minFilter.value_or(fastgltf::Filter::Linear))    |
            WrapSFilter(sampler.wrapS)                                         |
            WrapTFilter(sampler.wrapT);
    }

    WNODISCARD inline fastgltf::Asset LoadGltf(std::string_view file_path) {
        fastgltf::Parser parser {};

        constexpr auto gltfOptions =
            fastgltf::Options::DontRequireValidAssetMember |
            fastgltf::Options::AllowDouble                 |
            fastgltf::Options::LoadExternalBuffers;

        auto data = fastgltf::GltfDataBuffer::FromPath(file_path);

        assert(data);

        fastgltf::Asset asset_gltf;

        auto gltf_type = fastgltf::determineGltfFileType(data.get());
        std::filesystem::path path=file_path;

        if (fastgltf::GltfType::glTF == gltf_type) {
            auto load = parser.loadGltf(data.get(), path.parent_path(), gltfOptions);
            if (load) {
                asset_gltf = std::move(load.get());
            } else {
                throw std::runtime_error(
                    std::format(
                        "Failed while loading Gltf file : {}",
                        file_path
                        )
                    );
            }
        } else if (fastgltf::GltfType::GLB == gltf_type) {
            auto load = parser.loadGltfBinary(data.get(), path.parent_path(), gltfOptions);
            if (load) {
                asset_gltf = std::move(load.get());
            } else {
                throw std::runtime_error(
                    std::format(
                        "Failed while loading Gltf binary file: {}",
                        file_path
                        )
                    );
            }
        } else {
            throw std::runtime_error(
                std::format("Invalid Gltf type!")
                );
        }

        return asset_gltf;
    }

    WNODISCARD inline
    WRenderPipelineParametersAsset CollectMaterial(
        fastgltf::Asset const & in_asset,
        std::vector<NullableIndex<>> const & textures_map,
        std::vector<wid::WAssetId> const & textures,
        fastgltf::Material const & in_material,
        wid::WAssetId null_texture=wid::null_id,
        wid::WAssetId null_rgba=wid::null_id,
        wid::WAssetId null_normal=wid::null_id
        ) {
        WRenderPipelineParametersAsset result{};

        auto GetTextureId = [&textures, &textures_map]
            (auto & tex_info, wid::WAssetId fallback) -> wid::WAssetId {
            return tex_info
                .and_then(
                    [&textures, &textures_map, &fallback]
                    (fastgltf::TextureInfo const & value) -> std::optional<wid::WAssetId> {
                        if(textures_map[value.textureIndex].IsValid())
                            return textures[textures_map[value.textureIndex].GetId()];
                        else
                            return fallback;
                    }
                    ).value_or(fallback);
        };

        wct::render::WRPParameterList_WAssetId texture_params{};
        
        texture_params.emplace_back(1,
                                    GetTextureId(
                                        in_material
                                        .pbrData
                                        .baseColorTexture,
                                        null_rgba));
        
        texture_params.emplace_back(2,
                                    GetTextureId(
                                        in_material
                                        .emissiveTexture,
                                        null_texture));
        
        texture_params.emplace_back(3,
                                    GetTextureId(
                                        in_material
                                        .normalTexture,
                                        null_normal));

        if (in_material.packedOcclusionRoughnessMetallicTextures) {
            texture_params.emplace_back(
                4,
                GetTextureId(in_material
                             .packedOcclusionRoughnessMetallicTextures
                             ->roughnessMetallicOcclusionTexture,
                             null_texture));
        }
        else {
            texture_params.emplace_back(4, null_texture);
        }
            
        // TODO pbr values into UBO

        result.Set_texture_list(texture_params);

        return result;
    }

    WNODISCARD inline
    auto CollectMaterials(
        fastgltf::Asset const & in_asset,
        std::vector<NullableIndex<>> const & textures_map,
        std::vector<wid::WAssetId> const & texture_wids,
        wid::WAssetId null_texture=wid::null_id,
        wid::WAssetId null_rgba=wid::null_id,
        wid::WAssetId null_normal=wid::null_id
        ){
        
        std::vector<WRenderPipelineParametersAsset> assets;
        assets.reserve(in_asset.materials.size());

        std::vector<std::string_view> names;
        names.reserve(in_asset.materials.size());

        for(auto & mat : in_asset.materials) {
            assets.push_back(
                CollectMaterial(
                    in_asset,
                    textures_map,
                    texture_wids,
                    mat,
                    null_texture,
                    null_rgba,
                    null_normal
                    )
                );

            names.push_back(mat.name);
        }
        
        return std::tuple{std::move(assets),
                          std::move(names)};
    }

    WNODISCARD inline
    wct::geometry::WMesh CollectMeshPrimitive(
        fastgltf::Asset const & in_asset,
        fastgltf::Primitive const  & in_primitive
        ) {

        wct::geometry::WMesh result;

        {
            const fastgltf::Accessor& index_accessor =
                in_asset.accessors.at(in_primitive.indicesAccessor.value());

            result.indices.reserve(index_accessor.count);

            fastgltf::iterateAccessor<std::uint32_t>(
                in_asset, index_accessor,
                [&indices=result.indices]
                (std::uint32_t idx) {
                    indices.push_back(idx);
                }
                );
        }
            
        // Get vertex positions
        {
            const fastgltf::Accessor & pos_accessor =
                in_asset.accessors.at(in_primitive.findAttribute("POSITION")->accessorIndex);

            result.vertices.resize(pos_accessor.count);

            fastgltf::iterateAccessorWithIndex<glm::vec3>(
                in_asset, pos_accessor,
                [&vertex=result.vertices]
                (auto && vert_pos, std::size_t index) {
                    vertex[index].position = vert_pos;
                }
                );
        }

        // Get normals
        auto normals = in_primitive.findAttribute("NORMAL");
        if (normals != in_primitive.attributes.end()) {
            fastgltf::iterateAccessorWithIndex<glm::vec3>(
                in_asset, in_asset.accessors[normals->accessorIndex],
                [&vertex=result.vertices]
                (auto && normal, std::size_t index) {
                    vertex[index].normal = normal;
                }
                );
        }

        // Get Tex Coords 0
        {
            auto uv = in_primitive.findAttribute("TEXCOORD_0");
            if (uv != in_primitive.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec2>(
                    in_asset, in_asset.accessors[uv->accessorIndex],
                    [&vertex=result.vertices]
                    (auto && coords, std::size_t index) {
                        vertex[index].tex_coords = coords;
                    }
                    );
            }
        }

        // Get colors
        {
            auto colors = in_primitive.findAttribute("COLOR_0");
            if (colors != in_primitive.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec4>(
                    in_asset, in_asset.accessors[colors->accessorIndex],
                    [&vertex=result.vertices]
                    (auto && color, std::size_t index) {
                        vertex[index].color = color;
                    }
                    );
            }
        }

        // ...

        return result;
    }

    WNODISCARD inline
    auto CollectStaticMeshes(
        fastgltf::Asset const & in_asset,
        wid::WAssetId gbuffer_pipeline,
        wid::WAssetId null_pipe_params,
        wid::WAssetId transparent_pipeline,
        std::vector<wid::WAssetId> const & parameters
        ) {
        std::vector<NullableIndex<>> index_sm_map{};
        index_sm_map.reserve(in_asset.meshes.size());

        std::vector<WStaticMeshAsset> sm_assets;
        sm_assets.reserve(in_asset.meshes.size());

        std::vector<std::string_view> sm_names;
        sm_names.reserve(in_asset.meshes.size());

        for (fastgltf::Mesh const & mesh : in_asset.meshes) {

            WStaticMeshAsset sm_asset{};

            std::size_t prim_indx=0;
            while(prim_indx < mesh.primitives.size() &&
                  prim_indx < sm_asset.Get_meshes().max_size()) {

                auto & primitive = mesh.primitives[prim_indx];

                sm_asset.SetMesh(
                    CollectMeshPrimitive(
                        in_asset,
                        primitive
                        ),
                    prim_indx
                    );

                if (primitive.materialIndex &&
                    primitive.materialIndex.value() < parameters.size()) {
                    sm_asset.SetPipelineAssignment(
                        {
                            gbuffer_pipeline,  
                            parameters[primitive.materialIndex.value()]
                        },
                        prim_indx
                        );
                }
                else {
                    sm_asset.SetPipelineAssignment(
                        {gbuffer_pipeline,
                         null_pipe_params
                        },
                        prim_indx
                        );
                }

                prim_indx++;
            }

            if (prim_indx > 0) {
                index_sm_map.push_back(sm_assets.size());
                sm_assets.push_back(std::move(sm_asset));
                sm_names.push_back(mesh.name);
            }
            else {
                index_sm_map.push_back(wid::null_id);
            }
        }

        return std::tuple{std::move(index_sm_map),
                          std::move(sm_assets),
                          std::move(sm_names)};
    }

    WNODISCARD inline
    WTextureAsset CollectImage(
        fastgltf::Asset const & in_asset,
        fastgltf::Image const & in_image
        ) {
        WTextureAsset result;

        std::visit(
            fastgltf::visitor {
                [&result](auto const & arg) {},
                [&result](fastgltf::sources::URI const & filePath) {
                    assert(filePath.fileByteOffset == 0);
                    assert(filePath.uri.isLocalPath());

                    std::string_view path(
                        filePath.uri.path().begin(),
                        filePath.uri.path().end()
                        );

                    auto stbi_image = wim::WLib_wtbi::LoadPath(path);

                    if (stbi_image.pixels) {

                        result.SetTextureData(
                            stbi_image.pixels.get(),
                            stbi_image.width,
                            stbi_image.height,
                            stbi_image.format
                            );
                    }
                },
                [&result](fastgltf::sources::Vector const & vector) {

                    auto stbi_image = wim::WLib_wtbi::LoadBuffer(
                        vector.bytes.data(),
                        vector.bytes.size()
                        );

                    if (stbi_image.pixels) {
                        result.SetTextureData(
                            stbi_image.pixels.get(),
                            stbi_image.width,
                            stbi_image.height,
                            stbi_image.format
                            );
                    }
                },
                [&result, &in_asset](fastgltf::sources::BufferView const & view) {
                    auto& bufferView = in_asset.bufferViews[view.bufferViewIndex];
                    auto& buffer = in_asset.buffers[bufferView.bufferIndex];

                    std::visit(fastgltf::visitor { 
                            [](auto const & arg) {},
                            [&result, &bufferView]<typename T>
                            requires (std::same_as<T, fastgltf::sources::Vector> ||
                                std::same_as<T, fastgltf::sources::Array>)
                            (T const & container) {
                                auto stbi_image = wim::WLib_wtbi::LoadBuffer(
                                    container.bytes.data() + bufferView.byteOffset,
                                    bufferView.byteLength
                                    );

                                if (stbi_image.pixels) {

                                    result.SetTextureData(
                                        stbi_image.pixels.get(),
                                        stbi_image.width,
                                        stbi_image.height,
                                        stbi_image.format
                                        );
                                }
                            }
                        },
                        buffer.data
                        );
                }
                },
            in_image.data
            );

        return result;
    }

    WNODISCARD inline
    auto CollectTextureSamplersIndex(
        fastgltf::Asset const & in_asset
        ) {
        using OptIndex = decltype(decltype(in_asset.textures)::value_type::samplerIndex);
        using IndexType = std::decay_t<decltype(std::declval<OptIndex>().value())>;

        std::vector<NullableIndex<IndexType>> texture_samplers;
        texture_samplers.resize(in_asset.textures.size(), {});

        for(auto & tx : in_asset.textures) {
            tx.samplerIndex
                .and_then([](auto& v) ->std::optional<NullableIndex<IndexType>>
                          {return v;})
                .value_or(wid::null_id);

            // if (tx.imageIndex.has_value()) {
            //     image_samplers[tx.imageIndex.value()] =
            //         tx.samplerIndex
            //         .and_then([](auto & v ) -> std::optional<NullableIndex<IndexType>>
            //                   { return v ;})
            //         .value_or(wid::null_id);
            // }
        }
        return texture_samplers;
    }

    WNODISCARD inline
    auto CollectTextures(
        fastgltf::Asset const & in_asset
        ) {
        std::vector<NullableIndex<>> text_assets_map{};
        text_assets_map.resize(in_asset.textures.size(), wid::null_id);

        std::vector<WTextureAsset> text_assets{};
        text_assets.reserve(in_asset.textures.size());

        std::vector<std::string_view> text_names{};
        text_names.reserve(in_asset.textures.size());

        std::size_t idx=0;

        for (auto & text : in_asset.textures) {
            if (text.imageIndex.has_value()) {
                auto imgidx = text.imageIndex.value();

                auto const & img = in_asset.images[imgidx];

                text_assets.push_back(CollectImage(in_asset, img));

                text_assets.back().AddRGBAPadding();
                
                text_names.push_back(wstr::CleanBasename(text.name));

                if (text.samplerIndex.has_value()) {
                    auto sampleridx = text.samplerIndex.value();
                    
                    text_assets.back().Set_sampler(
                        ToESampler(in_asset.samplers[sampleridx])
                        );
                }
                text_assets_map[idx] = text_assets.size()-1;
            } 

            idx++;
        }

        return std::tuple{std::move(text_assets_map),
                          std::move(text_assets),
                          std::move(text_names)};
    }


    inline
    void CollectTransformComponent(
        WTransformComponent & out_transform,
        fastgltf::Node const & in_node
        ) {
        std::visit(
            fastgltf::visitor{
                [&out_transform](fastgltf::TRS const & value) {
                    out_transform.Set_position(
                        {value.translation.x(),
                         value.translation.y(),
                         value.translation.z()}
                        );
                    out_transform.Set_rotation(
                        {value.rotation.x(),
                         value.rotation.y(),
                         value.rotation.z()}
                        );
                    out_transform.Set_scale(
                        {value.scale.x(),
                         value.scale.y(),
                         value.scale.z()
                        }
                        );
                },
                [&out_transform](fastgltf::math::fmat4x4 const & value) {
                    
                    out_transform.SetTransformMatrix(
                        {value.col(0).x(), value.col(0).y(), value.col(0).z(), value.col(0).w(),
                         value.col(1).x(), value.col(1).y(), value.col(1).z(), value.col(1).w(),
                         value.col(2).x(), value.col(2).y(), value.col(2).z(), value.col(2).w(),
                         value.col(3).x(), value.col(3).y(), value.col(3).z(), value.col(3).w()
                        }
                        );
                }
                },
            in_node.transform
            );
    }

    inline
    void CollectLightComponent(
        wid::WEntityId in_entity,
        was::Level & out_level,
        fastgltf::Light const & in_light
        ) {
        auto collect_directional_lgt =
            [&](){
                // TODO Rename Light components less verbose
                out_level.CreateComponent<wcm::light::WDirectionalLightComponent>(in_entity);
                auto & cmp = out_level
                    .GetComponent<wcm::light::WDirectionalLightComponent>(in_entity);

                cmp.Set_color({in_light.color.x(), in_light.color.y(), in_light.color.z()});
                cmp.Set_intensity(in_light.intensity);
            };

        auto collect_point_lgt =
            [&](){
                out_level.CreateComponent<wcm::light::WPointLightComponent>(in_entity);
                auto & cmp = out_level
                    .GetComponent<wcm::light::WPointLightComponent>(in_entity);

                cmp.Set_color({in_light.color.x(), in_light.color.y(), in_light.color.z()});
                cmp.Set_intensity(in_light.intensity);
            };

        switch(in_light.type) {
        case fastgltf::LightType::Point:
            collect_point_lgt();
        case fastgltf::LightType::Directional:
            collect_directional_lgt();
        default:
            collect_point_lgt();
        }
    }

    inline
    void CollectCameraComponent(
        wid::WEntityId in_entity,
        was::Level & out_level,
        fastgltf::Camera const & in_camera
        ) {
        out_level.CreateComponent<WCameraComponent>(in_entity);
        auto & component = out_level.GetComponent<WCameraComponent>(in_entity);

        std::visit(
            fastgltf::visitor{
                [&component](fastgltf::Camera::Perspective const & value) {
                    component.Set_field_of_view(value.yfov);
                    if (value.zfar.has_value())
                        component.Set_far_clipping(value.zfar.value());

                    component.Set_near_clipping(value.znear);
                },
                    [](fastgltf::Camera::Orthographic const & value) {

                    }
                    },
            in_camera.camera
            );
        
    }

    WNODISCARD inline
    was::Level CollectLevel(
        fastgltf::Scene const & in_scene,
        fastgltf::Asset const & in_asset,
        std::vector<NullableIndex<>> const & sm_id_map,
        std::vector<wid::WAssetId> const & sm_wids,
        WAssetDb const & in_asset_db
        ) {
        
        was::Level level{};
        level.Set_name(in_scene.name.c_str());
        
        for (auto & nodeindex : in_scene.nodeIndices) {
            auto entityid = level.CreateEntity<WEntity>();

            auto trnsfid = level.CreateComponent<WTransformComponent>(entityid);
            auto & transform = level.GetComponent<WTransformComponent>(entityid);

            CollectTransformComponent(
                transform,
                in_asset.nodes[nodeindex]
                );

            if (in_asset.nodes[nodeindex].meshIndex.has_value()) {
                std::size_t gltfindx = in_asset.nodes[nodeindex].meshIndex.value();
                
                if (sm_id_map[gltfindx].IsValid()) {
                    auto smcmpid = level.CreateComponent<WStaticMeshComponent>(entityid);
                    auto & smcmp = level.GetComponent<WStaticMeshComponent>(entityid);

                    smcmp.SetStaticMeshAsset(
                        in_asset_db
                        .Get<WStaticMeshAsset>(sm_wids[sm_id_map[gltfindx].GetId()])
                        );
                }
            }

            if (in_asset.nodes[nodeindex].lightIndex.has_value()) {
                std::size_t gltfindx = in_asset.nodes[nodeindex].lightIndex.value();

                CollectLightComponent(
                    entityid,
                    level,
                    in_asset.lights[gltfindx]
                    );
            }

            if (in_asset.nodes[nodeindex].cameraIndex.has_value()) {
                std::size_t gltfindx = in_asset.nodes[nodeindex].cameraIndex.value();

                CollectCameraComponent(
                    entityid,
                    level,
                    in_asset.cameras[gltfindx]
                    );
            }
        }

        return level;
    }

    WNODISCARD inline
    auto CollectLevels(
        fastgltf::Asset const & in_asset,
        std::vector<NullableIndex<>> const & sm_id_map,
        std::vector<wid::WAssetId> const & sm_wids,
        WAssetDb const & in_asset_db
        ) {
        std::vector<was::Level> levels;
        std::vector<std::string_view> names;
        
        levels.reserve(in_asset.scenes.size());
        names.reserve(in_asset.scenes.size());

        for(fastgltf::Scene const & scene : in_asset.scenes) {
            levels.push_back(
                CollectLevel(
                    scene,
                    in_asset,
                    sm_id_map,
                    sm_wids,
                    in_asset_db
                    )
                );

            names.push_back(scene.name);
        }

        return std::tuple{levels, names};
    }

    WNODISCARD inline
    std::vector<wid::WAssetId> CreateLevels(
        std::vector<was::Level> & levels,
        std::vector<std::string_view> const & names,
        std::string_view assets_directory,
        WAssetDb & asset_db
        ) {

        std::vector<wid::WAssetId> result{};
        result.reserve(levels.size());
        
        for (std::uint32_t i=0; i<levels.size(); i++) {

            std::array<std::string, 2> name =
                asset_db.GenValidAssetName<was::Level>(
                    assets_directory,
                    names[i],
                    names[i]);

            result.push_back(
                asset_db.CreateFrom<was::Level>(
                    wstr::AssetPath(
                        assets_directory,
                        name[0],
                        name[1]
                        ),
                    std::move(levels[i])
                    )
                );
        }

        return result;
    }

    WNODISCARD inline
    std::vector<wid::WAssetId> CreateTextures(
        std::vector<WTextureAsset> & in_textures,
        std::vector<std::string_view> const & in_names,
        std::string_view assets_directory,
        WAssetDb & asset_db
        ) {

        std::vector<wid::WAssetId> result;
        result.reserve(in_textures.size());

        for(std::size_t i=0; i<in_textures.size(); i++) {

            std::array<std::string, 2> name =
                asset_db.GenValidAssetName<WTextureAsset>(
                    assets_directory,
                    in_names[i],
                    in_names[i]);
            
            result.push_back(
                asset_db.CreateFrom<WTextureAsset>(
                    wstr::AssetPath(
                        assets_directory,
                        name[0],
                        name[1]
                        ),
                    std::move(in_textures[i])
                    )
                );
        }

        return result;
    }
    
    WNODISCARD inline
    std::vector<wid::WAssetId> CreatePipelineParameters(
        std::vector<WRenderPipelineParametersAsset> & parameters,
        std::vector<std::string_view> const & names,
        std::string_view asset_directory,
        WAssetDb & asset_db
        ) {

        std::vector<wid::WAssetId> result;
        result.reserve(parameters.size());

        for(std::size_t i=0; i<parameters.size(); i++) {

            std::array<std::string, 2> name =
                asset_db.GenValidAssetName<WRenderPipelineParametersAsset>(
                    asset_directory,
                    names[i],
                    names[i]);

            result.push_back(
                asset_db
                .CreateFrom<WRenderPipelineParametersAsset>(
                    wstr::AssetPath(
                        asset_directory,
                        name[0], name[1]
                        ),
                    std::move(parameters[i])
                    )
                );
        }
        return result;
    }

    WNODISCARD inline
    std::vector<wid::WAssetId> CreateStaticMeshes(
        std::vector<WStaticMeshAsset> const & static_meshes,
        std::vector<std::basic_string_view<char>> const & names,
        std::string_view asset_directory,
        WAssetDb & asset_db
        ) {
        std::vector<wid::WAssetId> result;
        result.reserve(static_meshes.size());

        for (std::size_t i=0; i<static_meshes.size(); i++) {

            std::array<std::string, 2> name =
                asset_db.GenValidAssetName<WStaticMeshAsset>(
                    asset_directory,
                    names[i],
                    names[i]);

            result.push_back(
                asset_db
                .CreateFrom<WStaticMeshAsset>(
                    wstr::AssetPath(
                        asset_directory,
                        name[0], name[1]
                        ),
                    std::move(static_meshes[i])
                    ));
        }
        return result;
    }
}

std::vector<wid::WAssetId> wim::importer::WImporterGltf::Import(
    WAssetDb & in_asset_db,
    std::string_view file_path,
    std::string_view engine_directory_prefix
    ) {

    fastgltf::Asset gltf_asset=LoadGltf(file_path);

    // Materials and textures
    
    auto [textures_map, text_assets, textures_names] = CollectTextures(gltf_asset);

    // Create Texture Assets
    auto textures_wids = CreateTextures(
        text_assets,
        textures_names,
        engine_directory_prefix,
        in_asset_db
        );

    auto [mat_assets, mat_names] =CollectMaterials(
        gltf_asset,
        textures_map,
        textures_wids,
        textures_.null_texture,
        textures_.null_rgba,
        textures_.null_normal
        );

    auto materials_wid = CreatePipelineParameters(
        mat_assets,
        mat_names,
        engine_directory_prefix,
        in_asset_db
        );

    // Collect meshes
    auto [sm_index_map, sm_assets, sm_names] = CollectStaticMeshes(
        gltf_asset,
        render_pipelines_.pbr_opaque,
        render_pipelines_.pbr_param,
        render_pipelines_.transparent,
        materials_wid
        );

    auto sm_wid = CreateStaticMeshes(
        sm_assets,
        sm_names,
        engine_directory_prefix,
        in_asset_db
        );

    auto [level_assets, level_names] = CollectLevels(
        gltf_asset,
        sm_index_map,
        sm_wid,
        in_asset_db
        );

    auto lvl_wid = CreateLevels(
        level_assets,
        level_names,
        engine_directory_prefix,
        in_asset_db);
    
    std::vector<wid::WAssetId> result;
    result.reserve(textures_wids.size() +
                   materials_wid.size() +
                   sm_wid.size() +
                   lvl_wid.size());

    result.insert(result.end(),
                  std::make_move_iterator(textures_wids.begin()),
                  std::make_move_iterator(textures_wids.end()));
    
    result.insert(result.end(),
                  std::make_move_iterator(materials_wid.begin()),
                  std::make_move_iterator(materials_wid.end()));
    
    result.insert(result.end(),
                  std::make_move_iterator(sm_wid.begin()),
                  std::make_move_iterator(sm_wid.end()));
    
    result.insert(result.end(),
                  std::make_move_iterator(lvl_wid.begin()),
                  std::make_move_iterator(lvl_wid.end()));
    
    return result;
}

