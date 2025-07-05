#include "WSpacers.hpp"

#include "WEngine.hpp"
#include "WLog.hpp"
#include "WRender.hpp"
#include "WCore/WCore.hpp"
#include "WImporters.hpp"
#include "WImportersRegister.hpp"
#include "WActors/WActor.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WAssets/WTextureAsset.hpp"
#include "WVulkan/WVkRenderConfig.h"
#include "WVulkan/WVkRenderCore.hpp"
#include "WVulkan/WVkRenderPipeline.hpp"
#include "WStructs/WGeometryStructs.hpp"
#include "WVulkan/WVulkan.hpp"
#include "IRenderResources.hpp"

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


bool UpdateUniformBuffers(
    const WVkSwapChainInfo & swap_chain_info_,
    WVkUniformBufferObjectInfo & uniform_buffer_object_info_
    )
{
    WVkUBOStruct ubo{};

    ubo.model = glm::rotate(
        glm::mat4(1.f),
        glm::radians(90.f),
        glm::vec3(0.f, 0.f, 1.f)
        );
    ubo.view = glm::lookAt(
        glm::vec3(-2.f, 2.f, 2.f),
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

bool run(WEngine & engine)
{
    engine.run();
    return true;
}

WModelStruct MeshPlane()
{
    WModelStruct model;
    WMeshStruct mesh;

    mesh.vertices.push_back({
            glm::vec3(-1.f, -1.f, 0.f),
            glm::vec2(0.f, 0.f),
            glm::vec3(0.f, 0.f, 1.f)
        });

    mesh.vertices.push_back({
            glm::vec3(-1.f, 1.f, 0.f),
            glm::vec2(0.f, 1.f),
            glm::vec3(0.f, 0.f, 1.f)
        });

    mesh.vertices.push_back({
            glm::vec3(1.f, 1.f, 0.f),
            glm::vec2(1.f, 1.f),
            glm::vec3(0.f, 0.f, 1.f)
        });

    mesh.vertices.push_back({
            glm::vec3(1.f, -1.f, 0.f),
            glm::vec2(1.f, 0.f),
            glm::vec3(0.f, 0.f, 1.f)
        });

    mesh.indices.push_back(0);
    mesh.indices.push_back(3);
    mesh.indices.push_back(1);
    mesh.indices.push_back(2);
    mesh.indices.push_back(1);
    mesh.indices.push_back(3);

    model.meshes.push_back(mesh);

    return model;
}

bool LoadAssets(WEngine & engine, WStaticMeshAsset *& out_static_model, WTextureAsset *& out_texture_asset)
{
    TOptionalRef<WImportObj> obj_importer =
        engine.ImportersRegister()->GetImporter<WImportObj>();

    std::vector<WAsset*> geo_assets =
        obj_importer->Import(
            "Content/Assets/Models/viking_room.obj", 
            "/Content/Assets/viking_room.viking_room"
            );

    if (geo_assets.size() < 1)
    {
        std::cout << "Failed to import geo_asset!" << std::endl;
        return false;
    }

    if (geo_assets[0]->GetClass() != WStaticMeshAsset::GetStaticClass())
    {
        std::cout << "geo_asset is not a static model!" << std::endl;
        return false;
    }

    TOptionalRef<WImportTexture> texture_importer =
        engine.ImportersRegister()->GetImporter<WImportTexture>();

    std::vector<WAsset*> tex_asset = texture_importer->Import(
        "Content/Assets/Textures/viking_room.png", 
        "/Content/Assets/viking_texture.viking_texture"
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

    out_static_model = static_cast<WStaticMeshAsset*>(geo_assets[0]);
    out_texture_asset = static_cast<WTextureAsset*>(tex_asset[0]);

    return true;

}

bool LoadShaders(std::vector<WVkShaderStageInfo> & out_shaders)
{
    // TODO create shaders from WEngine module, (or from pipelines).

    out_shaders.clear();
    out_shaders.reserve(2);

    out_shaders.push_back(
        WVulkan::CreateShaderStageInfo(
            std::filesystem::absolute("Content/Shaders/Spacers_ShaderBase.vert").c_str(),
            "main",
            EShaderType::Vertex
            )
        );

    out_shaders.push_back(
        WVulkan::CreateShaderStageInfo(
            std::filesystem::absolute("Content/Shaders/Spacers_ShaderBase.frag").c_str(),
            "main",
            EShaderType::Fragment
            )
        );

    return true;
}

int main(int argc, char** argv)
{
    bool as_plane=false;
    
    try
    {
        WEngine engine = WEngine::Create();

        std::vector<WVkShaderStageInfo> shaders;
        LoadShaders(shaders);

        // Render Pipeline

        WId descriptor_set_layout =
            engine.Render()->RenderPipelinesManager().CreateDescriptorSetLayout();

        WVkRenderPipelineInfo render_pipeline_info;
        render_pipeline_info.type = EPipelineType::Graphics;

        WId pipeline_wid = engine.Render()->RenderPipelinesManager().CreateRenderPipeline(
            render_pipeline_info,
            shaders,
            descriptor_set_layout
            );

        WStaticMeshAsset * static_mesh;
        WTextureAsset * texture_asset;

        if (!LoadAssets(engine, static_mesh, texture_asset))
        {
            return 1;
        }
	        
        const WTextureStruct & texture_data =
            texture_asset->GetTexture();

        engine.Render()->RenderResources()->RegisterStaticMesh(static_mesh);
        engine.Render()->RenderResources()->RegisterTexture(texture_asset);
        engine.Render()->RenderResources()->LoadStaticMesh(static_mesh->WID());
        engine.Render()->RenderResources()->LoadTexture(texture_asset->WID());

        WId did =
            engine.Render()->RenderPipelinesManager().CreateDescriptorSet(
                descriptor_set_layout
                );

        engine.Render()->AddPipelineBinding(
            pipeline_wid,
            did,
            static_mesh->WID()
            );

        WLOG("Bind Pipeline: " << pipeline_wid.GetId());

        // Update Descriptor Sets //

        const WVkDescriptorSetInfo descriptor_set =
            engine.Render()->RenderPipelinesManager().DescriptorSet(did);

        std::array<WVkUniformBufferObjectInfo, WENG_MAX_FRAMES_IN_FLIGHT> uniform_buffer_info{};

        for(auto & uniform_buffer : uniform_buffer_info)
        {
            WVulkan::Create(uniform_buffer, engine.Render()->DeviceInfo());
            UpdateUniformBuffers(engine.Render()->SwapChainInfo(), uniform_buffer);
        }

        for (int i=0; i<descriptor_set.descriptor_sets.size(); i++)
        {
            // TODO Move into Render process

            std::vector<VkWriteDescriptorSet> write_descriptor_sets{2};

            WVulkan::WVkWriteDescriptorSetUBOStruct ubo_struct{};

            ubo_struct.binding = 0;
            ubo_struct.uniform_buffer_info = uniform_buffer_info[i];
            ubo_struct.descriptor_set = descriptor_set.descriptor_sets[i];

            WVulkan::UpdateWriteDescriptorSet(
                write_descriptor_sets[0],
                ubo_struct
                );

            WVulkan::WVkWriteDescriptorSetTextureStruct texture_struct{};

            texture_struct.binding = 1;
            // TODO Fix this
            // texture_struct.texture_info = engine.Render()->RenderResources()->TextureInfo(TextureAsset->WID());
            texture_struct.descriptor_set = descriptor_set.descriptor_sets[i];

            WVulkan::UpdateWriteDescriptorSet(
                write_descriptor_sets[1],
                texture_struct
                );

            // Update // 
            
            WVulkan::UpdateDescriptorSets(
                write_descriptor_sets,
                engine.Render()->DeviceInfo()
                );
        }

        // Start while loop

        run(engine);

    }
    catch(const std::exception& e)
    {
        std::cout << std::string("[ERROR] ") + e.what() << std::endl;

        return 1;
    }

    return 0;

}

