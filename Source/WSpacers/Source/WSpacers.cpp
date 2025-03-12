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
#include "WStructs/WGeometryStructs.h"
#include "WVulkan.h"

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

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

    ubo.proj[1][1] *= -1;  // Fix OpenGL Y axis inversion

    std::memcpy(uniform_buffer_object_info_.mapped_data, &ubo, sizeof(ubo));

    return true;
}

void GetPlane()
{
    std::array<WVertexStruct, 4> vertex{};

    vertex[0] = WVertexStruct{
        {-1.0, 1.0, 0.0},
        {},
        {},
        {},
        {},
        {}
    };

    vertex[1] = WVertexStruct{
        {1.0, 1.0, 0.0},
        {},
        {},
        {},
        {},
        {}
    };

    vertex[2] = WVertexStruct{
        {-1.0, -1.0, 0.0},
        {},
        {},
        {},
        {},
        {}
    };

    vertex[3] = WVertexStruct{
        {1.0, -1.0, 0.0},
        {},
        {},
        {},
        {},
        {}
    };
    
}

bool run(WRender & in_render)
{
    while(!glfwWindowShouldClose(in_render.WindowInfo().window))
    {
        glfwPollEvents();
        in_render.Draw();
    }

    in_render.DeviceWaitIdle();

    return true;
}

bool LoadAssets(WStaticModel *& out_static_model, WTextureAsset *& out_texture_asset)
{
    std::vector<WAsset*> geo_asset = WImportObj().Import(
        "Content/Assets/Models/viking_room.obj", 
        "/Content/Assets/modelobj.modelobj"
    );

    if (geo_asset.size() < 1)
    {
        std::cout << "Failed to import geo_asset!" << std::endl;
        return false;
    }

    if (geo_asset[0]->GetClass() != WStaticModel::GetStaticClass())
    {
        std::cout << "geo_asset is not a static model!" << std::endl;
        return false;
    }

    std::vector<WAsset*> tex_asset = WImportTexture().Import(
        "Content/Assets/Textures/viking_room.png", 
        "/Content/Assets/texture.texture"
    );

    if (tex_asset.size() < 1)
    {
        std::cout << "Failed to import tex_asset!" << std::endl;
        return false;
    }

    if (tex_asset[0]->GetClass() != WTextureAsset::GetStaticClass())
    {
        std::cout << "tex_asset is not a texture!" << std::endl;
        return false;
    }

    out_static_model = static_cast<WStaticModel*>(geo_asset[0]);
    out_texture_asset = static_cast<WTextureAsset*>(tex_asset[0]);

    return true;

}

int main(int argc, char** argv)
{
    try
    {
        WRender render;

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

        WId pipeline_wid = render.RenderPipelinesManager().CreateRenderPipeline(
            render_pipeline_info,
            shaders,
            descriptor_set_layout
            ).WID();

        WStaticModel * static_model;
        WTextureAsset * texture_asset;

        if (!LoadAssets(static_model, texture_asset))
        {
            return 1;
        }
	
        const WModelStruct & model_data = static_model->GetModel();
        const WTextureStruct & texture_data = texture_asset->GetTexture();

        WMeshInfo mesh_info;

        WVulkan::Create(
            mesh_info,
            model_data.meshes[0],
            render.DeviceInfo(),
            render.RenderCommandPool().CommandPoolInfo()
            );

        WDescriptorSetInfo descriptor_set =
            render.RenderPipelinesManager().CreateDescriptorSet(
                descriptor_set_layout
                );

        render.RenderPipelinesManager().AddBinding(
            pipeline_wid, descriptor_set, mesh_info
            );

        // Update Descriptor Sets

        std::vector<WUniformBufferObjectInfo> uniform_buffer_info {
            descriptor_set.descriptor_sets.size()
        };

        for(auto & uniform_buffer : uniform_buffer_info)
        {
            WVulkan::Create(uniform_buffer, render.DeviceInfo());
            UpdateUniformBuffers(render.SwapChainInfo(), uniform_buffer);
        }

        for (int i=0; i<descriptor_set.descriptor_sets.size(); i++)
        {
            WVulkan::WVkWriteDescriptorSetUBOStruct ubo_struct{};
            std::vector<VkWriteDescriptorSet> write_descriptor_sets{1};

            ubo_struct.binding = 0; // Should increment or something
            ubo_struct.uniform_buffer_info = uniform_buffer_info[i];
            ubo_struct.descriptor_set = descriptor_set.descriptor_sets[i];

            WVulkan::UpdateWriteDescriptorSet(
                write_descriptor_sets[0],
                ubo_struct
                );

            WVulkan::UpdateDescriptorSets(
                write_descriptor_sets,
                render.DeviceInfo()
                );
        }

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

