#include "WEngine/WEngineDefaults.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "WCore/WId.hpp"
#include "WCoreTypes/WTexture.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WVulkan/WVkRender.hpp"
#include "WImporter/WImporterTexture.hpp"
#include "WImporter/WImporterObj.hpp"
#include "WImporter/WImporterGltf.hpp"
#include "WImporterRegister/WImporterRegister.hpp"
#include "WSystems/WSystems.hpp"

#include <memory>

namespace {

    void DefaultTextures(WAssetDb & out_db) {
        std::vector<std::uint8_t> color_data {
            2 * 2 * 4,
            0};

        WTextureAsset texture_asset{};
        texture_asset.SetTextureData(
            color_data.data(), 2, 2,
            wct::texture::ETextureFormat::RGBA8_SRGB
            );

        auto wid=out_db.CreateFrom<WTextureAsset>(
            weng::defaults::NULL_RGBA_TEXTURE_ASSET_PATH,
            std::move(texture_asset)
            );

        color_data.assign(2 * 2 * 4, 127);
        std::ranges::fill(color_data.end() - 4, color_data.end(), 255);

        texture_asset = {};
        texture_asset.SetTextureData(
            color_data.data(), 2, 2,
            wct::texture::ETextureFormat::RGBA8_SNORM
            );

        wid=out_db.CreateFrom<WTextureAsset>(
            weng::defaults::NULL_NORMAL_TEXTURE_ASSET_PATH,
            std::move(texture_asset)
            );
    }

    void DefaultRenderPipelines(WAssetDb & out_db) {
        WRenderPipelineAsset pipeline_asset{};

        pipeline_asset.Set_pipeline_type(wct::render::EPipelineType::GBuffer);

        auto shader_stages = pipeline_asset.Get_shader_list();

        shader_stages[0].type=wct::render::EShaderStageFlag::Vertex;
        shader_stages[0].file = weng::defaults::PBR_PIPELINE_SHADER_PATH;
        shader_stages[0].entry = "vsMain";

        shader_stages[1].type = wct::render::EShaderStageFlag::Fragment;
        shader_stages[1].file = weng::defaults::PBR_PIPELINE_SHADER_PATH;
        shader_stages[1].entry = "fsMain";

        pipeline_asset.Set_shader_list(shader_stages);

        auto descriptors = pipeline_asset.Get_descriptor_list();

        descriptors[0].binding=0;
        descriptors[0].type=wct::render::EPipeParamType::Ubo;
        descriptors[0].stage_flags=wct::render::EShaderStageFlag::Vertex;
        descriptors[0].size=sizeof(wct::render::WModelUBO);

        // albedo
        descriptors[1].binding=1;
        descriptors[1].type=wct::render::EPipeParamType::Texture;
        descriptors[1].stage_flags=wct::render::EShaderStageFlag::Fragment;

        // emission
        descriptors[2].binding=2;
        descriptors[2].type=wct::render::EPipeParamType::Texture;
        descriptors[2].stage_flags=wct::render::EShaderStageFlag::Fragment;

        // normal map
        descriptors[3].binding=3;
        descriptors[3].type=wct::render::EPipeParamType::Texture;
        descriptors[3].stage_flags=wct::render::EShaderStageFlag::Fragment;

        // normal map
        descriptors[4].binding=4;
        descriptors[4].type=wct::render::EPipeParamType::Texture;
        descriptors[4].stage_flags=wct::render::EShaderStageFlag::Fragment;

        pipeline_asset.Set_descriptor_list(descriptors);
        
        out_db.CreateFrom<WRenderPipelineAsset>(
            weng::defaults::PBR_PIPELINE_ASSET_PATH,
            std::move(pipeline_asset)
            );
    }

}



WEngine weng::defaults::DefaultEngine() {
    
    WEngine result(std::make_unique<WVkRender>());

    result.ImportersRegister().Register<wim::importer::WImporterObj>();
    result.ImportersRegister().Register<wim::importer::WImportTexture>();
    
    result.RegSystems(WSystems::WENGINE_WSYSTEMS_REG);

    // This must be the first included system
    
    result.AddInitSystem(0, "SystemInit_InitializeTransformsMatrix");

    result.AddInitSystem(0, "SystemInit_RenderLevelResources");

    result.AddPostSystem(0, "SystemPost_UpdateRenderCamera");

    result.AddEndSystem(0, "SystemEnd_RenderLevelResources");

    // Default Assets

    DefaultTextures(result.AssetManager());

    DefaultRenderPipelines(result.AssetManager());

    // Gltf importer

    WAssetId pbr_pipeline_wid = result.AssetManager()
        .Get(weng::defaults::PBR_PIPELINE_ASSET_PATH)->Get_asset_id();

    result.ImportersRegister()
        .Register<wim::importer::WImporterGltf>(pbr_pipeline_wid, WID_NULL_V);

    // TODO Plugins Modules Loading

    return result;    
}
