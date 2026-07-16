#include "WEngine/WEngineDefaults.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "WAssets/WRenderPipelineParametersAsset.hpp"
#include "WCore/WId.hpp"
#include "WCoreTypes/WRenderTypes.hpp"
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

        out_db.CreateFrom<WTextureAsset>(
            weng::defaults::NULL_TEXTURE_ASSET_PATH,
            std::move(texture_asset)
            );

        // --

        color_data.assign(2*2*4, 255);
        std::ranges::fill(color_data.begin() + 4, color_data.begin() + 8, 127);
        std::ranges::fill(color_data.begin() + 8, color_data.begin() + 12, 0);

        texture_asset = {};
        texture_asset.SetTextureData(
            color_data.data(), 2, 2,
            wct::texture::ETextureFormat::RGBA8_SRGB
            );

        out_db.CreateFrom<WTextureAsset>(
            weng::defaults::NULL_RGBA_TEXTURE_ASSET_PATH,
            std::move(texture_asset)
            );
        
        // --

        color_data.assign(2 * 2 * 4, 127);
        std::ranges::fill(color_data.end() - 4, color_data.end(), 255);

        texture_asset = {};
        texture_asset.SetTextureData(
            color_data.data(), 2, 2,
            wct::texture::ETextureFormat::RGBA8_SNORM
            );

        out_db.CreateFrom<WTextureAsset>(
            weng::defaults::NULL_NORMAL_TEXTURE_ASSET_PATH,
            std::move(texture_asset)
            );
    }

    void DefaultRenderPipelines(WAssetDb & out_db) {
        WRenderPipelineAsset pipeline_asset{};

        pipeline_asset.Set_pipeline_type(wct::render::ERPipeType::GBuffer);

        auto shader_stages = pipeline_asset.Get_shader_list();

        shader_stages[0].type=wct::render::EShaderStageFlag::Vertex;
        shader_stages[0].file = weng::defaults::PBR_PIPELINE_SHADER_PATH;
        shader_stages[0].entry = "vsMain";

        shader_stages[1].type = wct::render::EShaderStageFlag::Fragment;
        shader_stages[1].file = weng::defaults::PBR_PIPELINE_SHADER_PATH;
        shader_stages[1].entry = "fsMain";

        pipeline_asset.Set_shader_list(shader_stages);

        auto descriptors = pipeline_asset.Get_descriptor_list();

        // Model UBO
        descriptors[0].binding=wct::render::CommonBindings::MODEL_UBO;
        descriptors[0].type=wct::render::ERPipeParamType::Ubo;
        descriptors[0].stage_flags=wct::render::EShaderStageFlag::Vertex;
        descriptors[0].size=sizeof(wct::render::ModelUBO);

        // PBR scalar params UBO
        descriptors[1].binding=wct::render::PBRBindings::PBR_SCALAR_UBO;
        descriptors[1].type=wct::render::ERPipeParamType::Ubo;
        descriptors[1].stage_flags=
            wct::render::EShaderStageFlag::Vertex |
            wct::render::EShaderStageFlag::Fragment ;
        descriptors[1].size=sizeof(wct::render::PBRScalarUBO);

        // albedo
        descriptors[2].binding=wct::render::PBRBindings::ALBEDO_TEXTURE;
        descriptors[2].type=wct::render::ERPipeParamType::Texture;
        descriptors[2].stage_flags=wct::render::EShaderStageFlag::Fragment;

        // emission
        descriptors[3].binding=wct::render::PBRBindings::EMISSION_TEXTURE;
        descriptors[3].type=wct::render::ERPipeParamType::Texture;
        descriptors[3].stage_flags=wct::render::EShaderStageFlag::Fragment;

        // normal map
        descriptors[4].binding=wct::render::PBRBindings::NORMAL_TEXTURE;
        descriptors[4].type=wct::render::ERPipeParamType::Texture;
        descriptors[4].stage_flags=wct::render::EShaderStageFlag::Fragment;

        // normal map
        descriptors[5].binding=wct::render::PBRBindings::MRAO_TEXTURE;
        descriptors[5].type=wct::render::ERPipeParamType::Texture;
        descriptors[5].stage_flags=wct::render::EShaderStageFlag::Fragment;

        pipeline_asset.Set_descriptor_list(descriptors);
        
        out_db.CreateFrom<WRenderPipelineAsset>(
            weng::defaults::PBR_PIPELINE_ASSET_PATH,
            std::move(pipeline_asset)
            );

        // TODO default pipeline parameters

        WRenderPipelineParametersAsset params{};
        
        params.Set_ubo_list(
            {
                
            }
            );
        params.Set_texture_list({
                {
                    wct::render::PBRBindings::ALBEDO_TEXTURE,
                    out_db.GetId(weng::defaults::NULL_RGBA_TEXTURE_ASSET_PATH)
                },
                {
                    wct::render::PBRBindings::EMISSION_TEXTURE,
                    out_db.GetId(weng::defaults::NULL_TEXTURE_ASSET_PATH)
                },
                {
                    wct::render::PBRBindings::NORMAL_TEXTURE,
                    out_db.GetId(weng::defaults::NULL_NORMAL_TEXTURE_ASSET_PATH)
                },
                {
                    wct::render::PBRBindings::MRAO_TEXTURE,
                    out_db.GetId(weng::defaults::NULL_TEXTURE_ASSET_PATH)
                },
            });

        out_db.CreateFrom<WRenderPipelineParametersAsset>(
            weng::defaults::PBR_PIPE_PARAMS_NULL_ASSET_PATH,
            params
            );
    }

    void DefaultInputAssets(WAssetDb & asset_db) {
        wid::WAssetId cameramapping = asset_db.Create<WInputMappingAsset>(
            weng::defaults::CAMERA_MAPPING_ASSET_PATH
            );

        wid::WAssetId frontaction = asset_db.Create<WActionAsset>(
            weng::defaults::FRONT_ACTION_ASSET_PATH
            );

        wid::WAssetId backaction = asset_db.Create<WActionAsset>(
            weng::defaults::BACK_ACTION_ASSET_PATH
            );

        wid::WAssetId leftaction = asset_db.Create<WActionAsset>(
            weng::defaults::LEFT_ACTION_ASSET_PATH
            );

        wid::WAssetId rightaction = asset_db.Create<WActionAsset>(
            weng::defaults::RIGHT_ACTION_ASSET_PATH
            );

        wid::WAssetId mousemovement = asset_db.Create<WActionAsset>(
            weng::defaults::MOUSE_MOVEMENT_ACTION_ASSET_PATH
            );

        auto & mapping_asset = asset_db
            .Get<WInputMappingAsset>(cameramapping);

        WInputMap input_map = mapping_asset.Get_input_map();

        input_map[{EInputKey::Key_W, EInputMode::Press}] = {frontaction};
        input_map[{EInputKey::Key_W, EInputMode::Release}] = {frontaction};
        input_map[{EInputKey::Key_S, EInputMode::Press}] = {backaction};
        input_map[{EInputKey::Key_S, EInputMode::Release}] = {backaction};
        input_map[{EInputKey::Key_A, EInputMode::Press}] = {leftaction};
        input_map[{EInputKey::Key_A, EInputMode::Release}] = {leftaction};
        input_map[{EInputKey::Key_D, EInputMode::Press}] = {rightaction};
        input_map[{EInputKey::Key_D, EInputMode::Release}] = {rightaction};
        input_map[{EInputKey::Mouse_Move, EInputMode::None}] = {mousemovement};

        mapping_asset.Set_input_map(input_map);

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

    DefaultInputAssets(result.AssetManager());

    // Gltf importer
    result.ImportersRegister()
        .Register<wim::importer::WImporterGltf>(
            result.AssetManager().GetId(weng::defaults::PBR_PIPELINE_ASSET_PATH),
            result.AssetManager().GetId(weng::defaults::PBR_PIPE_PARAMS_NULL_ASSET_PATH),
            wid::null_id,
            result.AssetManager().GetId(weng::defaults::NULL_TEXTURE_ASSET_PATH),
            result.AssetManager().GetId(weng::defaults::NULL_RGBA_TEXTURE_ASSET_PATH),
            result.AssetManager().GetId(weng::defaults::NULL_NORMAL_TEXTURE_ASSET_PATH)
            );

    // TODO Plugins Modules Loading

    return result;    
}
