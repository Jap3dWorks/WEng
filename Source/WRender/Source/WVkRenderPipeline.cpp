#include "WVulkan/WVkRenderPipeline.hpp"
#include "WStructs/WRenderStructs.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WCore/WStringUtils.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVkRenderStructs.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WVulkan/WVkRenderStructs.hpp"
#include "WLog.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cassert>
#include <utility>

// WRenderPipelinesManager
// -------------------

WVkRenderPipelinesManager::~WVkRenderPipelinesManager()
{
    Destroy();
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
    WFLOG("Initialize Global Graphic Descriptors.");
    Initialize_GlobalGraphicDescriptors();

    WFLOG("Initialize Descriptor Pool");

    WVulkan::Create(descriptor_pool_info_, device_info_);
    Initialize_ClearLambdas();
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
    global_graphics_descsets_(std::move(other.global_graphics_descsets_)),
    device_info_(std::move(other.device_info_)),
    render_pass_info_(std::move(other.render_pass_info_)),
    width_(std::move(other.width_)),
    height_(std::move(other.height_))
{
    other.device_info_ = {};
    other.render_pass_info_ = {};
    other.descriptor_pool_info_ = {};
    other.global_graphics_descsets_ = {};
}

WVkRenderPipelinesManager & WVkRenderPipelinesManager::operator=(WVkRenderPipelinesManager && other) noexcept
{
    if (this != &other) {
        Clear();

        device_info_ = std::move(other.device_info_);
        render_pass_info_ = std::move(other.render_pass_info_);
        descriptor_pool_info_ = std::move(other.descriptor_pool_info_);

        global_graphics_descsets_ = std::move(other.global_graphics_descsets_);

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
        other.global_graphics_descsets_ = {};
    }

    return *this;
}

void WVkRenderPipelinesManager::CreateRenderPipeline(
    const WAssetId & in_id,
    const WRenderPipelineStruct & pipeline_struct
    ) {

    WVkRenderPipelineInfo render_pipeline_info;
    render_pipeline_info.type = pipeline_struct.type;

    std::vector<WVkShaderStageInfo> shaders;
    shaders.reserve(pipeline_struct.shaders_count);

    for (uint8_t i=0; i < pipeline_struct.shaders_count; i++) {
        shaders.push_back(
            WVulkan::CreateShaderStageInfo(
                WStringUtils::SystemPath(pipeline_struct.shaders[i].file).c_str(),
                "main",
                pipeline_struct.shaders[i].type
                )
            );
    }
    
    // use asset pipeline id too.
    CreateGraphicDescriptorSetLayout(in_id);

    pipelines_.CreateAt(
        in_id,
        [this,
         &render_pipeline_info,
         &shaders,
         &in_id] (const WAssetId & _id) -> auto {
            
            auto& d_set_layout = descriptor_set_layouts_.Get(in_id);

            WVulkan::Create(
                render_pipeline_info,
                device_info_,
                render_pass_info_,
                {
                    global_graphics_descsets_.descset_layout_info,
                    d_set_layout
                },
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
    //  TODO check errors at close
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
    assert(pipeline_pbindings_.contains(in_pipeline_id));

    WVkRenderPipelineInfo pipeline_info = RenderPipelineInfo(in_pipeline_id);

    // Create a Descriptor set by binding,
    //  So each binding can use a different ubo buffer (each actor has a different transform).

    CreateDescriptorSet(
        pipeline_info.descriptor_set_layout_id,
        component_id
        );

    WVkDescriptorSetInfo ds = DescriptorSet(component_id);

    // Lambda ensures NRVO and avoids moves
    //  Create Descriptor binding objects with default values
    auto f = [this, &ds] () {
        std::array<WVkDescriptorSetUBOBinding, WENG_MAX_FRAMES_IN_FLIGHT> bindings;

        std::array<VkWriteDescriptorSet, ds.descriptor_sets.size()> write_ds;

        for(uint32_t i = 0; i < bindings.size(); i++) {
            
            bindings[i].binding = 0;
            bindings[i].ubo_info.range = sizeof(WUBOModelStruct);

            WVulkan::Create(bindings[i].ubo_info, device_info_);

            WVulkan::UpdateUBOModel(
                bindings[i].ubo_info,
                // Initial Position
                glm::rotate(
                    glm::mat4(1.f),
                    glm::radians(90.f),
                    glm::vec3(0.f, 0.f, 1.f)
                    ));

            bindings[i].buffer_info.buffer = bindings[i].ubo_info.uniform_buffer;
            bindings[i].buffer_info.offset = 0;
            bindings[i].buffer_info.range = bindings[i].ubo_info.range;

            WVulkan::UpdateWriteDescriptorSet_UBO(
                write_ds[i],
                bindings[i].binding,
                bindings[i].buffer_info,
                ds.descriptor_sets[i]
                );
        }

        WVulkan::UpdateDescriptorSets(
            write_ds,
            device_info_
            );

        return bindings;
    };

    auto t = [this,
              &in_textures,
              &in_textures_bindings,
              &ds] () {
        std::vector<WVkDescriptorSetTextureBinding> tx{in_textures.size()};
        std::vector<VkWriteDescriptorSet> write_ds{in_textures.size() * ds.descriptor_sets.size()};
        
        for (uint32_t i=0; i<ds.descriptor_sets.size(); i++) {
            for (uint32_t j = 0; j<tx.size(); j++) {

                tx[j].image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                tx[j].binding = in_textures_bindings[j];
                tx[j].image_info.imageView = in_textures[j].image_view;
                tx[j].image_info.sampler = in_textures[j].sampler;

                WVulkan::UpdateWriteDescriptorSet_Texture(
                    write_ds[(i * tx.size()) + j],
                    tx[j].binding,
                    tx[j].image_info,
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

    bindings_.CreateAt(
        component_id,
        [&in_pipeline_id,
         &in_mesh_asset_id,
         &component_id,
         &t,
         &f]
        (const WId & in_id) -> WVkPipelineBindingInfo
            {
                return {
                    in_id,
                    in_pipeline_id,
                    component_id,
                    in_mesh_asset_id,
                    t(),  // Create Texture descriptor sets
                    f()   // Create Ubo Model descriptor sets
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

    descriptor_sets_.Clear();
    if (descriptor_pool_info_.descriptor_pool != VK_NULL_HANDLE)
    {
        WVulkan::Destroy(descriptor_pool_info_, device_info_);
        descriptor_pool_info_.descriptor_pool = VK_NULL_HANDLE;
    }

    descriptor_set_layouts_.Clear();

    // Recreate the descriptor pool
    if (device_info_.vk_device != VK_NULL_HANDLE) {
        WVulkan::Create(descriptor_pool_info_, device_info_);
    }
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

    Destroy_GlobalGraphics();

    device_info_ = {};
    render_pass_info_ = {};

    width_=0;
    height_=0;
}

void WVkRenderPipelinesManager::CreateGraphicDescriptorSetLayout(const WAssetId & in_id) {

    WVkDescriptorSetLayoutInfo descriptor_set_layout_info;

    WVulkan::AddDSL_DefaultGraphicBindings(descriptor_set_layout_info);

    descriptor_set_layouts_.CreateAt(
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

void WVkRenderPipelinesManager::CreateDescriptorSet(
    const WAssetId & in_descriptor_set_layout_id,
    const WEntityComponentId & entity_component_id
    )
{
    auto & d_set_layout = descriptor_set_layouts_.Get(in_descriptor_set_layout_id);
    WVkDescriptorSetInfo descriptor_set_info;

    descriptor_sets_.CreateAt(
        entity_component_id,
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
            
            // descriptor_set_info.wid = in_id;
            return descriptor_set_info;
        }
        );
}

void WVkRenderPipelinesManager::Initialize_ClearLambdas() {

    // Destroy UBOs lambda
    bindings_.SetDestroyFn([di_=device_info_](auto & b) {
        WLOG("[PipelineManager] Destroy binding UBOs");
        for(auto& ubo: b.ubo) {
            WVulkan::Destroy(ubo.ubo_info, di_);
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

void WVkRenderPipelinesManager::Initialize_GlobalGraphicDescriptors() {

    WVulkan::AddDSL_DefaultGlobalGraphicBindings(
        global_graphics_descsets_.descset_layout_info
        );

    WVulkan::Create(
        global_graphics_descsets_.descset_layout_info,
        device_info_
        );
    
    WVulkan::Create(
        global_graphics_descsets_.descpool_info,
        device_info_
        );

    WVulkan::Create(
        global_graphics_descsets_.descset_info,
        device_info_,
        global_graphics_descsets_.descset_layout_info,
        global_graphics_descsets_.descpool_info
        );

    std::array<VkWriteDescriptorSet, WENG_MAX_FRAMES_IN_FLIGHT> ws;

    for(uint32_t i=0; i < WENG_MAX_FRAMES_IN_FLIGHT; i++) {
        global_graphics_descsets_.camera_ubo[i].range = sizeof(WUBOCameraStruct);

        WVulkan::Create(
            global_graphics_descsets_.camera_ubo[i],
            device_info_
            );    

        VkDescriptorBufferInfo buffer_info{};

        buffer_info.buffer = global_graphics_descsets_.camera_ubo[i].uniform_buffer;
        buffer_info.offset = 0;
        buffer_info.range = global_graphics_descsets_.camera_ubo[i].range;

        WVulkan::UpdateWriteDescriptorSet_UBO(
            ws[i],
            0,
            buffer_info,
            global_graphics_descsets_.descset_info.descriptor_sets[i]
            );
    }

    WVulkan::UpdateDescriptorSets(
        ws,
        device_info_
        );

}

void WVkRenderPipelinesManager::Destroy_GlobalGraphics() {

    if (device_info_.vk_device) {

        WVulkan::Destroy(
            global_graphics_descsets_.descpool_info,
            device_info_);

        for(uint32_t i=0; i<global_graphics_descsets_.camera_ubo.size(); i++) {
            WVulkan::Destroy(
                global_graphics_descsets_.camera_ubo[i],
                device_info_);
        }

        if (global_graphics_descsets_
            .descset_layout_info
            .descriptor_set_layout)
        {
            WVulkan::Destroy(
                global_graphics_descsets_.descset_layout_info,
                device_info_
                );
        }
    }

    global_graphics_descsets_ = GlobalGraphicsDescriptors();
}

void WVkRenderPipelinesManager::UpdateGlobalGraphicsDescriptorSet(
    const WUBOCameraStruct & camera_struct,
    uint32_t in_frame_index
    ) {

    WFLOG("Update Graphics DSet");

    memcpy(
        global_graphics_descsets_.camera_ubo[in_frame_index].mapped_data,
        &camera_struct,
        sizeof(WUBOCameraStruct)
        );
}
