#include "WImporter/WImporterGltf.hpp"
#include "WCoreTypes/WTexture.hpp"
#include "fastgltf/core.hpp"
#include "fastgltf/types.hpp"
#include "fastgltf/tools.hpp"

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

}


std::vector<WAssetId> wim::importer::WImporterGltf::Import(
    WAssetDb & in_asset_manager,
    std::string_view in_file_path,
    std::string_view in_asset_directory
    ) {

    fastgltf::Parser parser {};

    constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember |
        fastgltf::Options::AllowDouble |
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
                std::format("Failed while loading Gltf file : {}", in_file_path)
                );
        }
    } else if (fastgltf::GltfType::GLB == gltf_type) {
        auto load = parser.loadGltfBinary(data, path.parent_path(), gltfOptions);
        if (load) {
            asset_gltf = std::move(load.get());
        } else {
            throw std::runtime_error(
                std::format("Failed while loading Gltf binary file: {}", in_file_path)
                );
        }
    } else {
        throw std::runtime_error(
            std::format("Invalid Gltf type!")
            );
    }

    // --

    for (fastgltf::Mesh& mesh : asset_gltf.meshes) {

        mesh.name;

        for(auto& p : mesh.primitives) {
            // For each submesh (primitive)
            // continous memory, with indices to delimitate, it is cool.
            
            asset_gltf.accessors[p.indicesAccessor.value()].count;

            // Load indices

            {
                fastgltf::Accessor& indexaccessor =
                    asset_gltf.accessors[p.indicesAccessor.value()];
                indexaccessor.count; // indices

                fastgltf::iterateAccessor<std::uint32_t>(
                    asset_gltf, indexaccessor,
                    [&](std::uint32_t idx) {
                        // store indexes
                    }
                    );
            }

            // Load positions
            {
                
            }

            // load normals
            {
                
            }

            // load uvs
            {
                
            }

            // load colors
            {
                
            }
        }
    }

    return {};
}

