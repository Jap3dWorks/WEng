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
#include <GLFW/glfw3.h>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <glm/ext/matrix_transform.hpp>
#include <vector>
#include <cstring>

#include <iostream>

void SetupRender(WRender & render)
{
    // Shader Stages

    std::vector<WShaderStageInfo> shaders;
    shaders.reserve(2);

    shaders.push_back(
        WVulkan::CreateShaderStageInfo(
            std::filesystem::absolute("Content/Shaders/Spacers_ShaderBase.vert").c_str(),
            "main",
            EShaderType::Vertex
            )
        );

    shaders.push_back(
        WVulkan::CreateShaderStageInfo(
            std::filesystem::absolute("Content/Shaders/Spacers_ShaderBase.frag").c_str(),
            "main",
            EShaderType::Fragment
            )
        );

    // Render Pipeline

    WDescriptorSetLayoutInfo descriptor_set_layout =
        render.RenderPipelinesManager().CreateDescriptorSetLayout();

    WRenderPipelineInfo render_pipeline_info;
    render_pipeline_info.type = EPipelineType::Graphics;

    render.RenderPipelinesManager().CreateRenderPipeline(
        render_pipeline_info,
        shaders,
        descriptor_set_layout
	);
}

bool UpdateUniformBuffers(
    const WSwapChainInfo & swap_chain_info_,
    WUniformBufferObjectInfo & uniform_buffer_object_info_
    )
{
    WUniformBufferObject ubo{};

    ubo.model = glm::rotate(
        glm::mat4(1.f),
        glm::radians(90.f),
        glm::vec3(0.f, 0.f, 1.f)
        );
    ubo.view = glm::lookAt(
        glm::vec3(2.f, 2.f, 2.f),
        glm::vec3(0.f, 0.f, 0.f),
        glm::vec3(0.f, 0.f, 1.f)
        );
    ubo.proj = glm::perspective(
        glm::radians(45.f),
        swap_chain_info_.swap_chain_extent.width / (float) swap_chain_info_.swap_chain_extent.height,
        1.f, 10.f
        );

    ubo.proj[1][1] *= -1;  // Fix OpenGL Y inversion

    std::memcpy(uniform_buffer_object_info_.mapped_data, &ubo, sizeof(ubo));
}

bool run(WRender & in_render)
{

    std::vector<WUniformBufferObjectInfo> uniform_buffer_info {in_render.FramesInFlight()};

    for(auto& uniform_buffer : uniform_buffer_info)
    {
        WVulkan::Create(uniform_buffer, in_render.DeviceInfo());
        UpdateUniformBuffers(in_render.SwapChainInfo(), uniform_buffer);
    }

    while(!glfwWindowShouldClose(in_render.WindowInfo().window))
    {
        glfwPollEvents();
        in_render.Draw();
    }

    in_render.DeviceWaitIdle();

    return true;
}

int main(int argc, char** argv)
{
    try
    {
        WRender render;

        SetupRender(render);
	
        std::vector<WAsset*> geo_asset = WImportObj().Import(
            "Content/Assets/Models/viking_room.obj", 
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
            "Content/Assets/Textures/viking_room.png", 
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

    }
    catch(const std::exception& e)
    {
        std::cout << std::string("[ERROR] ") + e.what() << std::endl;

        return 1;
    }

    return 0;

}

