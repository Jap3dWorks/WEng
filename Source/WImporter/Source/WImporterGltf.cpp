#include "WImporter/WImporterGltf.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WCoreTypes/WGeometry.hpp"
#include "WCoreTypes/WTexture.hpp"
#include "WEngineInterfaces/IRender.hpp"
#include "fastgltf/core.hpp"
#include "fastgltf/types.hpp"
#include "fastgltf/tools.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WLib_stbi.hpp"

#include "glm/glm.hpp"
#include <glm/gtx/type_trait.hpp>

#include <stdexcept>

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

    inline wct::texture::ESampler ToESampler(fastgltf::Sampler & sampler) {
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

    WNODISCARD inline wct::geometry::WMesh CollectMeshPrimitive(
        const fastgltf::Asset & in_asset,
        const fastgltf::Primitive & in_primitive
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

    void CollectMaterials(fastgltf::Asset & gltf_asset) {
        
        for (fastgltf::Material& mat : gltf_asset.materials) {
            // std::shared_ptr<GLTFMaterial> newMat = std::make_shared<GLTFMaterial>();
            // materials.push_back(newMat);
            // file.materials[mat.name.c_str()] = newMat;

            // GLTFMetallic_Roughness::MaterialConstants constants;
            // constants.colorFactors.x = mat.pbrData.baseColorFactor[0];
            // constants.colorFactors.y = mat.pbrData.baseColorFactor[1];
            // constants.colorFactors.z = mat.pbrData.baseColorFactor[2];
            // constants.colorFactors.w = mat.pbrData.baseColorFactor[3];

            // constants.metal_rough_factors.x = mat.pbrData.metallicFactor;
            // constants.metal_rough_factors.y = mat.pbrData.roughnessFactor;
            // write material parameters to buffer
            
            // sceneMaterialConstants[data_index] = constants;

            // Detect if it is a transparent material
            // MaterialPass passType = MaterialPass::MainColor;
            // if (mat.alphaMode == fastgltf::AlphaMode::Blend) {
            //     passType = MaterialPass::Transparent;
            // }

            // GLTFMetallic_Roughness::MaterialResources materialResources;
            // default the material textures
            // materialResources.colorImage = engine->_whiteImage;
            // materialResources.colorSampler = engine->_defaultSamplerLinear;
            // materialResources.metalRoughImage = engine->_whiteImage;
            // materialResources.metalRoughSampler = engine->_defaultSamplerLinear;

            // set the uniform buffer for the material data
            // materialResources.dataBuffer =
            //     file.materialDataBuffer.buffer;

            // materialResources.dataBufferOffset =
            //     data_index * sizeof(GLTFMetallic_Roughness::MaterialConstants);

            // // grab textures from gltf file
            // if (mat.pbrData.baseColorTexture.has_value()) {
            //     size_t img = gltf_asset.textures[mat.pbrData.baseColorTexture.value().textureIndex].imageIndex.value();
            //     size_t sampler = gltf_asset.textures[mat.pbrData.baseColorTexture.value().textureIndex].samplerIndex.value();

            //     materialResources.colorImage = images[img];
            //     materialResources.colorSampler = file.samplers[sampler];
            // }
            // // build material
            // newMat->data = engine->metalRoughMaterial.write_material(engine->_device, passType, materialResources, file.descriptorPool);

            // data_index++;
        }        
    }

    void CollectLevel() {

        // // load all nodes and their meshes
        // for (fastgltf::Node& node : gltf.nodes) {
        //     std::shared_ptr<Node> newNode;

        //     // find if the node has a mesh, and if it does hook it to the mesh pointer and allocate it with the meshnode class
        //     if (node.meshIndex.has_value()) {
        //         newNode = std::make_shared<MeshNode>();
        //         static_cast<MeshNode*>(newNode.get())->mesh = meshes[*node.meshIndex];
        //     } else {
        //         newNode = std::make_shared<Node>();
        //     }

        //     nodes.push_back(newNode);
        //     file.nodes[node.name.c_str()];

        //     std::visit(fastgltf::visitor { [&](fastgltf::Node::TransformMatrix matrix) {
        //         memcpy(&newNode->localTransform, matrix.data(), sizeof(matrix));
        //     },
        //                 [&](fastgltf::Node::TRS transform) {
        //                     glm::vec3 tl(transform.translation[0], transform.translation[1],
        //                                  transform.translation[2]);
        //                     glm::quat rot(transform.rotation[3], transform.rotation[0], transform.rotation[1],
        //                                   transform.rotation[2]);
        //                     glm::vec3 sc(transform.scale[0], transform.scale[1], transform.scale[2]);

        //                     glm::mat4 tm = glm::translate(glm::mat4(1.f), tl);
        //                     glm::mat4 rm = glm::toMat4(rot);
        //                     glm::mat4 sm = glm::scale(glm::mat4(1.f), sc);

        //                     newNode->localTransform = tm * rm * sm;
        //                 } },
        //         node.transform);
        // }        
    }

    WTextureAsset CollectImage(fastgltf::Asset& asset, fastgltf::Image& image) {
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
                    [&](fastgltf::sources::Vector& vector) {
                        
                        // unsigned char* data =
                        //     stbi_load_from_memory(
                        //         vector.bytes.data(),
                        //         static_cast<int>(vector.bytes.size()),
                        //         &width, &height, &nrChannels, 4);
                        
                        // if (data) {
                        //     // VkExtent3D imagesize;
                        //     // imagesize.width = width;
                        //     // imagesize.height = height;
                        //     // imagesize.depth = 1;

                        //     // newImage = engine->create_image(data, imagesize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT,false);

                        //     stbi_image_free(data);
                        // }
                    },
                    [&](fastgltf::sources::BufferView& view) {
                        // auto& bufferView = asset.bufferViews[view.bufferViewIndex];
                        // auto& buffer = asset.buffers[bufferView.bufferIndex];

                        // std::visit(fastgltf::visitor { 
                        //         [](auto& arg) {},
                        //             [&](fastgltf::sources::Vector& vector) {
                        //                 unsigned char* data = stbi_load_from_memory(
                        //                     vector.bytes.data() + bufferView.byteOffset,
                        //                     static_cast<int>(bufferView.byteLength),
                        //                     &width, &height, &nrChannels, 4);
                                        
                        //                 if (data) {
                                            
                        //                     // VkExtent3D imagesize;
                        //                     // imagesize.width = width;
                        //                     // imagesize.height = height;
                        //                     // imagesize.depth = 1;

                        //                     // newImage = engine->create_image(
                        //                     //     data, imagesize,
                        //                     //     VK_FORMAT_R8G8B8A8_UNORM,
                        //                     //     VK_IMAGE_USAGE_SAMPLED_BIT,
                        //                     //     false);

                        //                     stbi_image_free(data);
                                            
                        //                 }
                        //             } },
                        //     buffer.data);
                    },
                    },
            image.data);

        return result;
    }

}

std::vector<WAssetId> wim::importer::WImporterGltf::Import(
    WAssetDb & in_asset_manager,
    std::string_view in_file_path,
    std::string_view in_asset_directory
    ) {

    fastgltf::Asset gltf_asset=LoadGltf(in_file_path);

    // Materials and textures

    gltf_asset.materials.size();
    gltf_asset.samplers.size();
    gltf_asset.images.size();

    for (auto & img : gltf_asset.images) {
        // CollectImage(gltf_asset, img);
    }
    
    std::vector<wct::geometry::WMesh> meshes;
    meshes.reserve(gltf_asset.meshes.size());

    for (fastgltf::Mesh& mesh : gltf_asset.meshes) {

        // mesh.name;

        WStaticMeshAsset sm_asset{};

        std::uint8_t idx=0;
        while(idx < mesh.primitives.size() &&
              idx < sm_asset.Get_mesh_list().size()) {

            sm_asset.SetMesh(
                CollectMeshPrimitive(
                    gltf_asset,
                    mesh.primitives[idx]
                    ),
                idx
                );

            idx++;
        }
    }

    // Nodes are Entities, can be imported like a level.
    // Current state is not working with transform hierarchy
    // CollectLevel()

    return {};
}

