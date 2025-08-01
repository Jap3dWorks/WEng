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

WVkRenderPipelinesManager::~WVkRenderPipelinesManager()
{
    Clear();
}

WVkRenderPipelinesManager::WVkRenderPipelinesManager(
    WVkDeviceInfo device,
    WVkRenderPassInfo render_pass_info,
    uint32_t in_width,
    uint32_t in_height
) : device_info_(device),
    render_pass_info_(render_pass_info),
    width_(in_width),
    height_(in_height)
{
    WVulkan::Create(descriptor_pool_info_, device_info_);
    InitializeClearLambdas();
}

WVkRenderPipelinesManager::WVkRenderPipelinesManager(
    WVkRenderPipelinesManager && other
    ) noexcept :
    pipelines_(std::move(other.pipelines_)),
    descriptor_set_layouts_(std::move(other.descriptor_set_layouts_)),
    descriptor_sets_(std::move(other.descriptor_sets_)),
    bindings_(std::move(other.bindings_)),
    pipeline_pbindings_(std::move(other.pipeline_pbindings_)),
    ptype_pipelines_(std::move(other.ptype_pipelines_)),
    descriptor_pool_info_(std::move(other.descriptor_pool_info_)),
    device_info_(std::move(other.device_info_)),
    render_pass_info_(std::move(other.render_pass_info_)),
    width_(std::move(other.width_)),
    height_(std::move(other.height_))
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

        pipeline_pbindings_ = std::move(other.pipeline_pbindings_);
        ptype_pipelines_ = std::move(other.ptype_pipelines_);

        width_ = std::move(other.width_);
        height_ = std::move(other.height_);

        other.device_info_ = {};
        other.render_pass_info_ = {};
        other.descriptor_pool_info_ = {};

    }

    return *this;
}

void WVkRenderPipelinesManager::CreateRenderPipeline(
    const WAssetId & in_id,
    const WRenderPipelineStruct & pstruct
    ) {

    WVkRenderPipelineInfo render_pipeline_info;
    render_pipeline_info.type = pstruct.type;

    std::vector<WVkShaderStageInfo> shaders;
    shaders.reserve(pstruct.shaders.size());

    for (uint8_t i=0; i < pstruct.shaders_count; i++) {
        shaders.push_back(
            WVulkan::CreateShaderStageInfo(
                WStringUtils::SystemPath(pstruct.shaders[i].file).c_str(),
                "main",
                pstruct.shaders[i].type
                )
            );
    }
    
    // use asset pipeline id too.
    CreateDescriptorSetLayout(in_id);

    pipelines_.Insert(
        in_id,
        [this,
         &render_pipeline_info,
         &shaders,
         &in_id](const WAssetId & _id) -> auto {
            
            auto& d_set_layout = descriptor_set_layouts_.Get(in_id);

            WVulkan::Create(
                render_pipeline_info,
                device_info_,
                d_set_layout,
                render_pass_info_,
                shaders
                );
            
            render_pipeline_info.wid = in_id;
            render_pipeline_info.descriptor_set_layout_id = in_id;
            
            return render_pipeline_info;
        }
        );

    pipeline_pbindings_[in_id] = {};
    ptype_pipelines_[render_pipeline_info.type].Insert(in_id.GetId(), in_id);
}

void WVkRenderPipelinesManager::DeleteRenderPipeline(
    const WAssetId & in_id
    )
{
    // Remove bindings
    for (auto & bid : pipeline_pbindings_[in_id]) {
        bindings_.Remove(bid) ;
        descriptor_sets_.Remove(bid);
    }

    pipeline_pbindings_.erase(in_id);
    
    pipelines_.Remove(in_id);

    for(auto & p : ptype_pipelines_) {
        if (p.second.Contains(in_id.GetId())) {
            p.second.Delete(in_id.GetId());
        }
    }
}

WId WVkRenderPipelinesManager::CreateBinding(
    const WEntityComponentId & component_id,
    const WAssetId & in_pipeline_id,
    const WAssetId & in_mesh_asset_id,
    std::vector<WVkTextureInfo> in_textures,
    std::vector<uint16_t> in_textures_bindings
    ) noexcept
{
    assert(pipeline_bindings_.contains(in_pipeline_id));

    WVkRenderPipelineInfo pipeline_info = RenderPipelineInfo(in_pipeline_id);

    // Create a Descriptor set by binding,
    //  so each binding can use a different ubo buffer (each actor has a different transform).

    WId dset_id = CreateDescriptorSet(
        pipeline_info.descriptor_set_layout_id
        );

    WVkDescriptorSetInfo ds = DescriptorSet(dset_id);

    // Lambda ensures NRVO and avoids moves
    // Create Descriptor binding objects with default values
    auto f = [this, &ds] () {
        std::array<WVkDescriptorSetUBOBinding, WENG_MAX_FRAMES_IN_FLIGHT> b;

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
                    (float) width_ / (float) height_,
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

    auto t = [this,
              &in_textures,
              &in_textures_bindings,
              &ds] () {
        std::vector<WVkDescriptorSetTextureBinding> tx{in_textures.size()};
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

    bindings_.Insert(
        component_id,
        [&in_pipeline_id,
         &in_mesh_asset_id,
         &dset_id,
         &t,
         &f]
        (const WId & in_id) -> WVkPipelineBindingInfo
            {
                return {
                    in_id,
                    in_pipeline_id,
                    dset_id,
                    in_mesh_asset_id,
                    t(),  // Create Texture descriptor sets
                    f()   // Create Ubo descriptor sets
                };
            }
        );

    pipeline_pbindings_[in_pipeline_id].Insert(component_id.GetId(), component_id);

    return component_id;
}

void WVkRenderPipelinesManager::DeleteBinding(const WEntityComponentId & in_id) {

    bindings_.Remove(in_id);
    descriptor_sets_.Remove(in_id);

    for(auto & p : pipeline_pbindings_) {
        if (p.second.Contains(in_id.GetId())) {
            p.second.Delete(in_id.GetId());            
        }
    }
}

void WVkRenderPipelinesManager::ForEachPipeline(EPipelineType in_type,
                                                TFunction<void(const WAssetId &)> in_predicate)
{
    for(auto & wid : ptype_pipelines_[in_type]) {
        in_predicate(wid);
    }
}
    
void WVkRenderPipelinesManager::ForEachPipeline(EPipelineType in_type,
                                                TFunction<void(WVkRenderPipelineInfo&)> in_predicate)
{
    for(auto & wid : ptype_pipelines_[in_type]) {
        in_predicate(pipelines_.Get(wid));
    }
}
    
void WVkRenderPipelinesManager::ForEachBinding(const WAssetId & in_pipeline_id,
                                               TFunction<void(const WEntityComponentId &)> in_predicate) {
    for(auto & wid : pipeline_pbindings_[in_pipeline_id]) {
        in_predicate(wid);
    }
}
    
void WVkRenderPipelinesManager::ForEachBinding(const WAssetId & in_pipeline_id,
                                               TFunction<void(WVkPipelineBindingInfo)> in_predicate) {
    for (auto & wid : pipeline_pbindings_[in_pipeline_id]) {
        in_predicate(bindings_.Get(wid));
    }
}

void WVkRenderPipelinesManager::Clear()
{
    bindings_.Clear();
    pipelines_.Clear();

    // Recreate the descriptor pool recreates the descriptor sets
    
    descriptor_sets_.Clear();
    if (descriptor_pool_info_.descriptor_pool != VK_NULL_HANDLE)
    {
        // Also destroys descriptor sets
        WVulkan::Destroy(descriptor_pool_info_, device_info_);
        descriptor_pool_info_.descriptor_pool = VK_NULL_HANDLE;
    }
    WVulkan::Create(descriptor_pool_info_, device_info_);

    descriptor_set_layouts_.Clear();
}

void WVkRenderPipelinesManager::Destroy() {
    bindings_.Clear();
    pipelines_.Clear();

    // Destroy the descriptor pool destroys the descriptor sets
    
    descriptor_sets_.Clear();
    if (descriptor_pool_info_.descriptor_pool != VK_NULL_HANDLE)
    {
        WVulkan::Destroy(descriptor_pool_info_, device_info_);
        descriptor_pool_info_.descriptor_pool = VK_NULL_HANDLE;
    }
    
    descriptor_set_layouts_.Clear();

    device_info_ = {};
    render_pass_info_ = {};

    width_=0;
    height_=0;
}

void WVkRenderPipelinesManager::CreateDescriptorSetLayout(const WAssetId & in_id) {

    WVkDescriptorSetLayoutInfo descriptor_set_layout_info;

    WVulkan::AddDSLDefaultGraphicBindings(descriptor_set_layout_info);

    descriptor_set_layouts_.Insert(
        in_id,
        [this,
         &descriptor_set_layout_info](const WAssetId & _in_id) -> auto {
        WVulkan::Create(
            descriptor_set_layout_info,
            device_info_
            );

        descriptor_set_layout_info.wid = _in_id;
        return descriptor_set_layout_info;
    });
}

WEntityComponentId WVkRenderPipelinesManager::CreateDescriptorSet(
    const WEntityComponentId & in_descriptor_set_layout_id
    )
{
    // TODO One descriptor layout by render pipline?
    auto & d_set_layout = descriptor_set_layouts_.Get(in_descriptor_set_layout_id);
    WVkDescriptorSetInfo descriptor_set_info;

    return descriptor_sets_.Create(
        [this,
         &descriptor_set_info,
         &d_set_layout]
        (const WAssetId & in_id) {
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

void WVkRenderPipelinesManager::InitializeClearLambdas() {

    // Destroy UBOs lambda
    bindings_.SetDestroyFn([di_=device_info_](auto & b) {
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

    pipelines_.SetDestroyFn([di_=device_info_](auto & p) {
        WLOG("[PipelineManager] Destory Render Pipelines");
        WVulkan::Destroy(
            p,
            di_
            );
    });

    descriptor_set_layouts_.SetDestroyFn([di_=device_info_](auto & d) {
        WLOG("[PipelineManager] Destroy descriptor Set layouts");
        WVulkan::Destroy(
            d,
            di_
            );
    });
}

