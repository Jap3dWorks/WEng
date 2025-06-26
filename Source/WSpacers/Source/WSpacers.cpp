#include "WSpacers.h"

#include "WEngine.h"
#include "WLog.h"
#include "WRender.h"
#include "WCore/WCore.h"
#include "WImporters.h"
#include "WImportersRegister.h"
#include "WActors/WActor.h"
#include "WAssets/WStaticModel.h"
#include "WAssets/WTextureAsset.h"
#include "WVulkan/WVkRenderCore.h"
#include "WVulkan/WVkRenderPipeline.h"
#include "WStructs/WGeometryStructs.h"
#include "WVulkan/WVulkan.h"

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

bool LoadAssets(WEngine & engine, WStaticModel *& out_static_model, WTextureAsset *& out_texture_asset)
{
    TOptionalRef<WImportObj> obj_importer =
        engine.ImportersRegister()->GetImporter<WImportObj>();

    std::vector<WAsset*> geo_asset = obj_importer->Import(
        "Content/Assets/Models/viking_room.obj", 
        "/Content/Assets/viking_room.viking_room"
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

    TOptionalRef<WImportTexture> texture_importer =
        engine.ImportersRegister()->GetImporter<WImportTexture>();

    std::vector<WAsset*> tex_asset = texture_importer->Import(
        "Content/Assets/Textures/viking_room.png", 
        "/Content/Assets/viking_texture.viking_texture"
    );

    if (false)
    {
        std::vector<WAsset*> tex_asset_rgba = texture_importer->Import(
            "Content/Assets/Textures/viking_room_rgba.png",
            "/Content/Assets/viking_texture_rgba.viking_texture_rgba"
            );

        WTextureAsset * a = static_cast<WTextureAsset*>(tex_asset[0]);
        WTextureAsset * b = static_cast<WTextureAsset*>(tex_asset_rgba[0]);

        for (int i=0; i < a->GetTexture().data.size(); i++) {
            if (a->GetTexture().data[i] != b->GetTexture().data[i]) {
                WLOGFNAME("- [" << i << "] are diferent, " <<
                          (uint32_t)a->GetTexture().data[i] <<
                          " != " <<
                          (uint32_t)b->GetTexture().data[i]
                    );
            }
        }
    }

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

        WVkDescriptorSetLayoutInfo descriptor_set_layout =
            engine.Render()->RenderPipelinesManager().CreateDescriptorSetLayout();

        WVkRenderPipelineInfo render_pipeline_info;
        render_pipeline_info.type = EPipelineType::Graphics;

        WId pipeline_wid = engine.Render()->RenderPipelinesManager().CreateRenderPipeline(
            render_pipeline_info,
            shaders,
            descriptor_set_layout
            ).WID();

        WStaticModel * static_model;
        WTextureAsset * texture_asset;

        if (!LoadAssets(engine, static_model, texture_asset))
        {
            return 1;
        }
	
        for (auto& mesh : static_model->GetModel().meshes)
        {
            WLOG("A Mesh with: " << mesh.indices.size() << " Indices");
            WLOG("A Mesh with: " << mesh.vertices.size() << " Vertices");
        }

        WLOG("Texture Data Size: " << texture_asset->GetTexture().data.size());
        WLOG("Texture Width: " << texture_asset->GetTexture().width);
        WLOG("Texture Height: " << texture_asset->GetTexture().height);

        WModelStruct model_data;

        if (as_plane)
        {
            model_data = MeshPlane();
        }
        else
        {
            model_data = static_model->GetModel();
        }
        
        const WTextureStruct & texture_data = texture_asset->GetTexture();

        WVkMeshInfo mesh_info;
        WVulkan::Create(
            mesh_info,
            model_data.meshes[0],
            engine.Render()->DeviceInfo(),
            engine.Render()->RenderCommandPool().CommandPoolInfo()
            );        

        WVkTextureInfo texture_info;
        WVulkan::Create(
            texture_info,
            texture_data,
            engine.Render()->DeviceInfo(),
            engine.Render()->RenderCommandPool().CommandPoolInfo()
            );

        WLOG("Texture Sampler: " << texture_info.sampler);
        WLOG("Image Memory: " << texture_info.image_memory);
        WLOG("Image View: " << texture_info.image_view);
        WLOG("Image: " << texture_info.image);

        WVkDescriptorSetInfo descriptor_set =
            engine.Render()->RenderPipelinesManager().CreateDescriptorSet(
                descriptor_set_layout
                );

        engine.Render()->RenderPipelinesManager().AddBinding(
            pipeline_wid, descriptor_set, mesh_info
            );

        WLOG("Bind Pipeline: " << pipeline_wid);

        // Update Descriptor Sets //

        std::vector<WVkUniformBufferObjectInfo> uniform_buffer_info {
            descriptor_set.descriptor_sets.size()
        };

        for(auto & uniform_buffer : uniform_buffer_info)
        {
            WVulkan::Create(uniform_buffer, engine.Render()->DeviceInfo());
            UpdateUniformBuffers(engine.Render()->SwapChainInfo(), uniform_buffer);
        }

        for (int i=0; i<descriptor_set.descriptor_sets.size(); i++)
        {
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
            texture_struct.texture_info = texture_info;
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

