#include "WVulkan/WVkRenderPipeline.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WCore/WStringUtils.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderCore.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WVulkan/WVkRenderCore.hpp"
#include "WLog.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cassert>
#include <utility>

// WRenderPipelinesManager
// -------------------

WVkRenderPipelinesManager::WVkRenderPipelinesManager(
    WVkDeviceInfo device,
    WVkRenderPassInfo render_pass_info,
    uint32_t in_width,
    uint32_t in_height
) : device_info_(device),
    render_pass_info_(render_pass_info),
    width(in_width),
    height(in_height)
{
    InitializeClearLambdas();
}

WVkRenderPipelinesManager::~WVkRenderPipelinesManager()
{
    Clear();
}

WVkRenderPipelinesManager::WVkRenderPipelinesManager(
    WVkRenderPipelinesManager && other
    ) noexcept :
    pipelines_(std::move(other.pipelines_)),
    descriptor_set_layouts_(std::move(other.descriptor_set_layouts_)),
    descriptor_sets_(std::move(other.descriptor_sets_)),
    bindings_(std::move(other.bindings_)),
    pipeline_bindings_(std::move(other.pipeline_bindings_)),
    stage_pipelines_(std::move(other.stage_pipelines_)),
    descriptor_pool_info_(std::move(other.descriptor_pool_info_)),
    device_info_(std::move(other.device_info_)),
    render_pass_info_(std::move(other.render_pass_info_)),
    width(std::move(other.width)),
    height(std::move(other.height))
{
    other.device_info_ = {};
    other.render_pass_info_ = {};
    other.descriptor_pool_info_ = {};
}

WVkRenderPipelinesManager & WVkRenderPipelinesManager::operator=(WVkRenderPipelinesManager && other) noexcept
{
    if (this != &other) {
        Clear();

        device_info_ = std::move(other.device_info_);
        render_pass_info_ = std::move(other.render_pass_info_);
        descriptor_pool_info_ = std::move(other.descriptor_pool_info_);

        pipelines_ = std::move(other.pipelines_);
        descriptor_set_layouts_ = std::move(other.descriptor_set_layouts_);
        descriptor_sets_ = std::move(other.descriptor_sets_);
        bindings_ = std::move(other.bindings_);

        pipeline_bindings_ = std::move(other.pipeline_bindings_);
        stage_pipelines_ = std::move(other.stage_pipelines_);

        width = std::move(other.width);
        height = std::move(other.height);

        other.device_info_ = {};
        other.render_pass_info_ = {};
        other.descriptor_pool_info_ = {};

    }

    return *this;
}

WId WVkRenderPipelinesManager::CreateRenderPipeline(
    EPipelineType in_pipeline_type,
    WId in_descriptor_set_layout_id,
    const std::vector<std::string> & in_shader_files,
    const std::vector<EShaderType> & in_shader_types
    )
{
    // TODO also create Descriptor Set Here
    
    WVkRenderPipelineInfo render_pipeline_info;
    render_pipeline_info.type = in_pipeline_type;

    std::vector<WVkShaderStageInfo> shaders;
    shaders.reserve(in_shader_files.size());

    for(uint32_t i=0; i<in_shader_files.size(); i++) {
        shaders.push_back(
            WVulkan::CreateShaderStageInfo(
                WStringUtils::SystemPath(in_shader_files[i]).c_str(),
                "main",
                in_shader_types[i]
                )
            );
    }

    auto& d_set_layout = descriptor_set_layouts_.Get(in_descriptor_set_layout_id);

    WId pid = pipelines_.Create(
        [this,
         &render_pipeline_info,
         &d_set_layout,
         &shaders](WId in_id) -> auto {
            WVulkan::Create(
                render_pipeline_info,
                device_info_,
                d_set_layout,
                render_pass_info_,
                shaders
                );
            
            render_pipeline_info.wid = in_id;
            return render_pipeline_info;
        }
        );

    pipeline_bindings_[pid] = {};
    stage_pipelines_[render_pipeline_info.type].push_back(pid);

    return pid;
}

WId WVkRenderPipelinesManager::CreateDescriptorSetLayout() {

    WVkDescriptorSetLayoutInfo descriptor_set_layout_info;

    WVulkan::AddDSLDefaultGraphicBindings(descriptor_set_layout_info);

    return descriptor_set_layouts_.Create(
        [this,
         &descriptor_set_layout_info](WId _in_id) -> auto {
        WVulkan::Create(
            descriptor_set_layout_info,
            device_info_
            );

        descriptor_set_layout_info.wid = _in_id;
        return descriptor_set_layout_info;
    });
}

WId WVkRenderPipelinesManager::CreateDescriptorSet(
    WId in_descriptor_set_layout_id
    )
{
    auto & d_set_layout = descriptor_set_layouts_.Get(in_descriptor_set_layout_id);
    WVkDescriptorSetInfo descriptor_set_info;

    return descriptor_sets_.Create(
        [this,
         &descriptor_set_info,
         &d_set_layout]
        (WId in_id) {
            WVulkan::Create(
                descriptor_set_info,
                device_info_,
                d_set_layout,
                descriptor_pool_info_
                );
            
            descriptor_set_info.wid = in_id;
            return descriptor_set_info;
        }
        );
}

WId WVkRenderPipelinesManager::AddBinding(
    WId in_pipeline_id,
    WId in_descriptor_id,
    WId in_mesh_asset_id,
    std::vector<WVkTextureInfo> in_textures,
    std::vector<uint32_t> in_textures_bindings
    ) noexcept
{
    assert(pipeline_bindings_.contains(in_pipeline_id));

    // Create uniform buffers bindings
    // Lambda ensures NRVO and avoids moves

    // Create Descriptor binding objects with default values
    auto f = [this,
              &in_descriptor_id]
        () {
        std::array<WVkDescriptorSetUBOBinding, WENG_MAX_FRAMES_IN_FLIGHT> b;
        WVkDescriptorSetInfo ds = DescriptorSet(in_descriptor_id);
        std::array<VkWriteDescriptorSet, ds.descriptor_sets.size()> write_ds;

        for(uint32_t i = 0; i < b.size(); i++) {
            
            b[i].binding = 0;
            
            WVulkan::Create(b[i].uniform_buffer_info, device_info_);

            WVulkan::UpdateUniformBuffer(
                b[i].uniform_buffer_info,
                glm::rotate(
                    glm::mat4(1.f),
                    glm::radians(90.f),
                    glm::vec3(0.f, 0.f, 1.f)
                    ),
                glm::lookAt(
                    glm::vec3(-2.f, 2.f, 2.f),
                    glm::vec3(0.f, 0.f, 0.f),
                    glm::vec3(0.f, 0.f, 1.f)
                    ),
                glm::perspective(
                    glm::radians(45.f),
                    (float) width / (float) height,
                    1.f, 10.f
                    )
                );
            
            WVulkan::UpdateWriteDescriptorSet_UBO(
                write_ds[i],
                b[i],
                ds.descriptor_sets[i]
                );
            
        }
        
        WVulkan::UpdateDescriptorSets<write_ds.size()>(
            write_ds,
            device_info_
            );

        return b;
    };

    // Create Descriptor binding objects with default values
    auto t = [this, &in_textures, &in_textures_bindings, &in_descriptor_id] () {
        std::vector<WVkDescriptorSetTextureBinding> tx{in_textures.size()};
        WVkDescriptorSetInfo ds = DescriptorSet(in_descriptor_id);
        std::vector<VkWriteDescriptorSet> write_ds{in_textures.size() * ds.descriptor_sets.size()};
        
        for (uint32_t i=0; i<ds.descriptor_sets.size(); i++) {
            for (uint32_t j = 0; j<tx.size(); j++) {
                tx[j].binding = in_textures_bindings[j];
                tx[j].image_view = in_textures[j].image_view;
                tx[j].sampler = in_textures[j].sampler;

                WVulkan::UpdateWriteDescriptorSet_Texture(
                    write_ds[(i * tx.size()) + j],
                    tx[j],
                    ds.descriptor_sets[i]
                    );
            }
        }

        WVulkan::UpdateDescriptorSets(
            write_ds,
            device_info_
            );

        return tx;
    };

    // TODO user TSparseSet for bindings_, and the Actor WId as id.
    WId result = bindings_.Create(
        [&in_pipeline_id,
         &in_descriptor_id,
         &in_mesh_asset_id,
         &t,
         &f]
        (WId in_id) -> WVkPipelineBindingInfo
            {
                return {
                    in_id,                // TODO: use actor id binding
                    in_pipeline_id,
                    in_descriptor_id,
                    in_mesh_asset_id,
                    t(),
                    f()
                };
            }
        );

    pipeline_bindings_[in_pipeline_id].push_back(result);

    return result;
}

void WVkRenderPipelinesManager::Clear()
{
    bindings_.Clear();
    pipelines_.Clear();

    if (descriptor_pool_info_.descriptor_pool != VK_NULL_HANDLE)
    {
        // Also destroys descriptor sets
        WVulkan::Destroy(descriptor_pool_info_, device_info_);
        descriptor_pool_info_.descriptor_pool = VK_NULL_HANDLE;
    }

    descriptor_sets_.Clear();
    descriptor_set_layouts_.Clear();

    device_info_ = {};
    render_pass_info_ = {};

    width = 0;
    height = 0;
}

void WVkRenderPipelinesManager::InitializeClearLambdas() {

    WVulkan::Create(descriptor_pool_info_, device_info_);

    // Destroy UBOs lambda
    bindings_.SetClearFn([di_=device_info_](auto & b) {
        WLOG("[PipelineManager] Destroy binding UBOs");
        for(auto& ubo: b.ubo) {
            vkDestroyBuffer(di_.vk_device,
                            ubo.uniform_buffer_info.uniform_buffer,
                            nullptr);

            vkFreeMemory(di_.vk_device,
                         ubo.uniform_buffer_info.uniform_buffer_memory,
                         nullptr);
        }
    });

    pipelines_.SetClearFn([di_=device_info_](auto & p) {
        WLOG("[PipelineManager] Destory Render Pipelines");
        WVulkan::Destroy(
            p,
            di_
            );
    });

    descriptor_set_layouts_.SetClearFn([di_=device_info_](auto & d) {
        WLOG("[PipelineManager] Destroy descriptor Set layouts");
        WVulkan::Destroy(
            d,
            di_
            );
    });
}
