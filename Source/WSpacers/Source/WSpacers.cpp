#include "WSpacers.h"

#include "WEngine.h"
#include "WRender.h"
#include "WCore/WCore.h"
#include "WImporters.h"
#include "WActors/WActor.h"
#include "WAssets/WStaticModel.h"
#include "WAssets/WTextureAsset.h"
#include "WRenderCore.h"
#include "WRenderPipeline.h"
#include "WVulkan.h"
#include <exception>
#include <vector>

#include <iostream>

void SetupRender(WRender & render)
{
    // Shader Stages

    std::vector<WShaderStageInfo> shaders;

    shaders.push_back(
        WVulkan::CreateShaderStageInfo(
            "../Shaders/ShaderBase.vert",
            "main",
            WShaderType::Vertex
            )
        );

    shaders.push_back(
        WVulkan::CreateShaderStageInfo(
            "../Shaders/ShaderBase.frag",
            "main",
            WShaderType::Fragment
            )
        );

    // Render Pipeline

    WRenderPipelineInfo render_pipeline_info;

    WDescriptorSetLayoutInfo descriptor_set_layout;

    descriptor_set_layout = render.RenderPipelinesManager().CreateDescriptorSetLayout(
        descriptor_set_layout
	);

    render.RenderPipelinesManager().CreateRenderPipeline(
        render_pipeline_info,
        shaders,
        descriptor_set_layout
	);
}

bool run(WRender & in_render)
{
    while(true)
    {
        in_render.DrawFrame();
    }

    return true;
}

int main(int argc, char** argv)
{
    try
    {
        WRender render;

        SetupRender(render);
	
        std::vector<WAsset*> geo_asset = WImportObj().Import(
            "../Content/Assets/Models/viking_room.obj", 
            "/Content/Assets/modelobj.modelobj"
        );
    
        if (geo_asset.size() < 1)
        {
            std::cout << "Failed to import geo_asset!" << std::endl;
            return 1;
        }

        if (geo_asset[0]->GetClass() != WStaticModel::GetStaticClass())
        {
            std::cout << "geo_asset is not a static model!" << std::endl;
            return 1;
        }

        std::vector<WAsset*> tex_asset = WImportTexture().Import(
            "../Content/Assets/Textures/viking_room.png", 
            "/Content/Assets/texture.texture"
        );

        if (tex_asset.size() < 1)
        {
            std::cout << "Failed to import tex_asset!" << std::endl;
            return 1;
        }

        if (tex_asset[0]->GetClass() != WTextureAsset::GetStaticClass())
        {
            std::cout << "tex_asset is not a texture!" << std::endl;
            return 1;
        }
    
        WStaticModel* static_model = static_cast<WStaticModel*>(geo_asset[0]);
        const WModelStruct& ModelData = static_model->GetModel();

        WTextureAsset* texture_asset = static_cast<WTextureAsset*>(tex_asset[0]);
        texture_asset->GetTexture();

        // assign shader to models

        // start while loop

        run(render);

        render.DeviceWaitIdle();
    }
    catch(const std::exception& e)
    {
        std::cout << std::string("[ERROR] ") + e.what() << std::endl;

        return 1;
    }

    return 0;

}

