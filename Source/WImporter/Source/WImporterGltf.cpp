#include "WImporter/WImporterGltf.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WCore/WCore.hpp"
#include "WCoreTypes/WGeometry.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
#include "WCoreTypes/WTexture.hpp"
#include "WEngineInterfaces/IRender.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WString/WStringUtils.hpp"
#include "fastgltf/core.hpp"
#include "fastgltf/types.hpp"
#include "fastgltf/tools.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WRenderPipelineParametersAsset.hpp"

#include <WLib_stbi.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/type_trait.hpp>

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

namespace nullindex {

    struct _NullIndexType_ {};

    template<typename T=std::size_t, T NullValue=std::numeric_limits<T>::max()>
    struct NullableIndex {

        NullableIndex(T in_val) :
            value(in_val) {}

        constexpr NullableIndex(_NullIndexType_) :
            value(NullValue) {}

        NullableIndex() = default;
        NullableIndex(const NullableIndex&) = default;
        NullableIndex(NullableIndex&&) = default;
        NullableIndex& operator=(const NullableIndex&) = default;
        NullableIndex& operator=(NullableIndex&&) = default;
        virtual ~NullableIndex() = default;

        WNODISCARD bool IsValid() { return value != NullValue; }
        WNODISCARD operator bool() const {return IsValid(); }

        WNODISCARD T const & Value() const { return value; }
        WNODISCARD T const &  operator*() const { return value; }

    private:
        
        T value{NullValue};
    };

    constexpr const _NullIndexType_ Null={};
}

namespace {


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
            WrapSFilter(sampler.wrapS) |
            WrapTFilter(sampler.wrapT);
    }

    WNODISCARD inline fastgltf::Asset LoadGltf(std::string_view in_file_path) {
        fastgltf::Parser parser {};

        constexpr auto gltfOptions =
            fastgltf::Options::DontRequireValidAssetMember |
            fastgltf::Options::AllowDouble                 |
            fastgltf::Options::LoadExternalBuffers;

        fastgltf::GltfDataBuffer data;

        data.FromPath(in_file_path);

        fastgltf::Asset asset_gltf;

        auto gltf_type = fastgltf::determineGltfFileType(data);
        std::filesystem::path path=in_file_path;

        if (fastgltf::GltfType::glTF == gltf_type) {
            auto load = parser.loadGltf(data, path.parent_path(), gltfOptions);
            if (load) {
                asset_gltf = std::move(load.get());
            } else {
                throw std::runtime_error(
                    std::format(
                        "Failed while loading Gltf file : {}",
                        in_file_path
                        )
                    );
            }
        } else if (fastgltf::GltfType::GLB == gltf_type) {
            auto load = parser.loadGltfBinary(data, path.parent_path(), gltfOptions);
            if (load) {
                asset_gltf = std::move(load.get());
            } else {
                throw std::runtime_error(
                    std::format(
                        "Failed while loading Gltf binary file: {}",
                        in_file_path
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

    WNODISCARD inline  WRenderPipelineParametersAsset CollectMaterial(
        fastgltf::Asset const & in_asset,
        std::vector<WAssetId> const & textures,
        fastgltf::Material const & in_material
        ) {
        WRenderPipelineParametersAsset result{};

        // in_material.normalTexture;
        // in_material.emissiveTexture;
        // in_material.packedOcclusionRoughnessMetallicTextures;
        // in_material.pbrData.baseColorTexture;
        // in_material.pbrData.metallicRoughnessTexture;

        auto GetWAssetId = [&textures]
            (auto & tex_info) -> WAssetId {
            return tex_info
                .and_then(
                    [&textures]
                    (fastgltf::TextureInfo const & value) -> std::optional<WAssetId> {
                        return textures[value.textureIndex];
                    }
                    )
                .or_else(
                    []() -> std::optional<WAssetId>
                    { return wid_null; }
                    )
                .value();
        };

        wct::render::WRPParameterList_WAssetId texture_params{};
        texture_params.reserve(4);
        
        if (WAssetId albedo_wid = GetWAssetId(
            in_material.pbrData.baseColorTexture
                )) {
            texture_params.emplace_back(
                1,  // pbr binding constant
                albedo_wid
                );
        }

        if (WAssetId normal_wid = GetWAssetId(
                in_material.normalTexture
                )) {
            texture_params.emplace_back(
                2, // pbr binding constant
                normal_wid
                );
        }

        if (in_material.packedOcclusionRoughnessMetallicTextures) {

            if(WAssetId mrAO = GetWAssetId(
                   in_material
                   .packedOcclusionRoughnessMetallicTextures
                   ->roughnessMetallicOcclusionTexture
                   )) {
                texture_params.emplace_back(
                    3,
                    mrAO
                    );
            }
            
        }

        // TODO pbr values into UBO

        result.Set_texture_list(texture_params);

        return {};
    }

    WNODISCARD inline
    auto
    CollectMaterials(
        fastgltf::Asset const & in_asset,
        std::vector<WAssetId> const & in_textures
        ){
        
        std::vector<WRenderPipelineParametersAsset> assets;
        assets.reserve(in_asset.materials.size());

        std::vector<std::string_view> names;
        names.reserve(in_asset.materials.size());

        for(auto & mat : in_asset.materials) {
            assets.push_back(
                CollectMaterial(
                    in_asset,
                    in_textures,
                    mat
                    )
                );

            names.push_back(mat.name);
        }
        
        return std::tuple{std::move(assets),
                          std::move(names)};
    }

    WNODISCARD inline wct::geometry::WMesh CollectMeshPrimitive(
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

        return result;
    }

    WNODISCARD inline auto CollectStaticMeshes(
        fastgltf::Asset const & in_asset,
        WAssetId gbuffer_pipeline,
        WAssetId transparent_pipeline,
        std::vector<WAssetId> const & in_parameters
        ) {

        std::vector<nullindex::NullableIndex<>> index_sm_map;
        index_sm_map.reserve(in_asset.meshes.size());

        std::vector<WStaticMeshAsset> sm_assets;
        sm_assets.reserve(in_asset.meshes.size());

        std::vector<std::string_view> sm_names;
        sm_names.reserve(in_asset.meshes.size());

        for (fastgltf::Mesh const & mesh : in_asset.meshes) {

            WStaticMeshAsset sm_asset{};

            std::size_t idx=0;
            
            while(idx < mesh.primitives.size() &&
                  idx < sm_asset.Get_meshes().size()) {

                auto & primitive = mesh.primitives[idx];

                sm_asset.SetMesh(
                    CollectMeshPrimitive(
                        in_asset,
                        primitive
                        ),
                    idx
                    );

                if (primitive.materialIndex) {
                    sm_asset.SetPipelineAssignment(
                        {
                            // TODO, select valid pipeline
                            gbuffer_pipeline,  
                            in_parameters[primitive.materialIndex.value()]
                        },
                        idx
                        );
                }
                
                idx++;
            }

            if (idx > 0) {
                index_sm_map.push_back(sm_assets.size());
                sm_assets.push_back(sm_asset);
                sm_names.push_back(mesh.name);
            }
            else {
                index_sm_map.push_back(nullindex::Null);
            }
        }

        return std::tuple{std::move(index_sm_map),
                          std::move(sm_assets),
                          std::move(sm_names)};
    }

    WNODISCARD inline WTextureAsset CollectImage(
        fastgltf::Asset const & in_asset,
        fastgltf::Image const & in_image
        ) {
        WTextureAsset result;

        std::visit(
            fastgltf::visitor {
                [&result](auto& arg) {},
                [&result](fastgltf::sources::URI& filePath) {
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
                [&result](fastgltf::sources::Vector& vector) {

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
                [&result, &in_asset](fastgltf::sources::BufferView& view) {
                    auto& bufferView = in_asset.bufferViews[view.bufferViewIndex];
                    auto& buffer = in_asset.buffers[bufferView.bufferIndex];

                    std::visit(fastgltf::visitor { 
                            [](auto& arg) {},
                            [&result, &bufferView](fastgltf::sources::Vector& vector) {
                                auto stbi_image = wim::WLib_wtbi::LoadBuffer(
                                    vector.bytes.data() + bufferView.byteOffset,
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
                },
            },
            in_image.data
            );

        return result;
    }

    WNODISCARD inline auto CollectImageSamplersIndex(
        fastgltf::Asset const & in_asset
        ) {
        using OptIndex = decltype(decltype(in_asset.textures)::value_type::samplerIndex);
        using IndexType = std::decay_t<decltype(std::declval<OptIndex>().value())>;

        std::vector<nullindex::NullableIndex<IndexType>> image_samplers;
        image_samplers.resize(in_asset.images.size(), {});

        for(auto & tx : in_asset.textures) {
            if (tx.imageIndex.has_value()) {
                image_samplers[tx.imageIndex.value()] =
                    tx.samplerIndex
                    .and_then([](auto & v ) -> std::optional<nullindex::NullableIndex<IndexType>>
                              { return v ;})
                    .value_or(nullindex::Null);
            }
        }
        return image_samplers;
    }

    WNODISCARD inline auto CollectImages(
        fastgltf::Asset const & in_asset
        ) {
        auto image_samplers = CollectImageSamplersIndex(in_asset);

        std::vector<WTextureAsset> text_assets{};
        text_assets.reserve(in_asset.images.size());

        std::vector<std::string_view> text_names{};
        text_names.reserve(in_asset.images.size());

        std::size_t idx=0;
        for (auto & img : in_asset.images) {
            text_assets.push_back(
                CollectImage(in_asset, img)
                );

            text_names.push_back(
                wstr::utils::CleanBasename(img.name)
                );

            if(image_samplers[idx].IsValid()) {
                auto sampleridx = image_samplers[idx].Value();
                text_assets[idx].Set_sampler(
                    ToESampler(in_asset.samplers[sampleridx])
                    );
            }
        
            idx++;
        }

        return std::tuple{std::move(text_assets), std::move(text_names)};
    }

    WNODISCARD inline
    std::vector<WAssetId> CreateTextures(
        std::vector<WTextureAsset> & in_textures,
        std::vector<std::string_view> const & in_names,
        std::string_view in_engine_path,
        WAssetDb & asset_db
        ) {

        std::vector<WAssetId> result;
        result.reserve(in_textures.size());

        for(std::size_t i=0; i<in_textures.size(); i++) {

            auto assetid = asset_db.CreateFrom<WTextureAsset>(
                wstr::utils::AssetPath(
                    std::string(in_engine_path),
                    std::string(in_names[i]),
                    std::string(in_names[i])
                    ),
                std::move(in_textures[i])
                );

            result.push_back(assetid);
        }

        return result;
    }
    
    WNODISCARD inline
    std::vector<WAssetId> CreatePipelineParameters(
        std::vector<WRenderPipelineParametersAsset> & in_parameters,
        std::vector<std::string_view> const & in_names,
        std::string_view in_asset_directory,
        WAssetDb & in_asset_db
        ) {

        std::vector<WAssetId> result;
        result.reserve(in_parameters.size());

        for(std::size_t i=0; i<in_parameters.size(); i++) {
            auto assetid = in_asset_db
                .CreateFrom<WRenderPipelineParametersAsset>(
                    wstr::utils::AssetPath(
                        std::string(in_asset_directory),
                        std::string(in_names[i]),
                        std::string(in_names[i])
                        ),
                    std::move(in_parameters[i])
                    );
        }
        return result;
    }

    WNODISCARD inline
    std::vector<WAssetId> CreateStaticMeshes(
        std::vector<WStaticMeshAsset> const & in_static_meshes,
        std::vector<std::basic_string_view<char>> const & in_names,
        std::string_view in_asset_directory,
        WAssetDb & in_asset_db
        ) {
        std::vector<WAssetId> result;
        result.reserve(in_static_meshes.size());

        for (std::size_t i=0; i<in_static_meshes.size(); i++) {
            auto assetid = in_asset_db
                .CreateFrom<WStaticMeshAsset>(
                    wstr::utils::AssetPath(
                        std::string(in_asset_directory),
                        std::string(in_names[i]),
                        std::string(in_names[i])
                        ),
                    std::move(in_static_meshes[i])
                    );
        }
        return result;
    }

}

std::vector<WAssetId> wim::importer::WImporterGltf::Import(
    WAssetDb & in_asset_db,
    std::string_view in_file_path,
    std::string_view in_asset_directory
    ) {

    fastgltf::Asset gltf_asset=LoadGltf(in_file_path);
    std::uint32_t idx=0;

    // TODO create a pbr shader
    // Materials and textures
    
    // gltf_asset.materials.size();

    // gltf_asset.samplers.size();
    // gltf_asset.images.size();

    // Collect textures

    auto textures_data = CollectImages(gltf_asset);

    // Create Texture Assets
    auto textures_wids = CreateTextures(
        std::get<0>(textures_data),
        std::get<1>(textures_data),
        in_asset_directory,
        in_asset_db
        );

    // TODO Collect Materials
    auto materials_data = CollectMaterials(
        gltf_asset,
        textures_wids
        );

    auto materials_wid = CreatePipelineParameters(
        std::get<0>(materials_data),
        std::get<1>(materials_data),
        in_asset_directory,
        in_asset_db
        );

    // Collect meshes
    auto static_mesh_data = CollectStaticMeshes(
        gltf_asset,
        render_pipelines_.gbuffer,
        render_pipelines_.transparent,
        materials_wid
        );

    auto sm_wid = CreateStaticMeshes(
        std::get<1>(static_mesh_data),
        std::get<2>(static_mesh_data),
        in_asset_directory,
        in_asset_db
        );
    
    // Nodes are Entities, can be imported like a level.
    // Current state is not working with transform hierarchy
    // CollectLevel()

    return {};
}

