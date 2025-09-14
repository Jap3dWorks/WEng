#include "WVulkan/WVkGraphicsPipelines.hpp"
#include "WStructs/WRenderStructs.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WCore/WStringUtils.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WLog.hpp"
#include "WVulkan/WDescriptorPoolUtils.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cassert>
#include <utility>

// WRenderPipelinesManager
// -------------------

WVkGraphicsPipelines::~WVkGraphicsPipelines()
{
    Destroy();
}

WVkGraphicsPipelines::WVkGraphicsPipelines(
    WVkDeviceInfo device,
    uint32_t in_width,
    uint32_t in_height
) : device_info_(device),
    width_(in_width),
    height_(in_height)
{
    WFLOG("Initialize Global Graphic Descriptors.");
    Initialize_GlobalGraphicDescriptors();
    Initialize_ClearLambdas();
}

WVkGraphicsPipelines::WVkGraphicsPipelines(
    WVkGraphicsPipelines && other
    ) noexcept :
    pipelines_(std::move(other.pipelines_)),
    descriptor_set_layouts_(std::move(other.descriptor_set_layouts_)),
    descriptor_pools_(std::move(other.descriptor_pools_)),
    bindings_(std::move(other.bindings_)),
    pipeline_pbindings_(std::move(other.pipeline_pbindings_)),
    global_graphics_descsets_(std::move(other.global_graphics_descsets_)),
    device_info_(std::move(other.device_info_)),
    width_(std::move(other.width_)),
    height_(std::move(other.height_))
{
    other.device_info_ = {};
    other.global_graphics_descsets_ = {};
}

WVkGraphicsPipelines & WVkGraphicsPipelines::operator=(WVkGraphicsPipelines && other) noexcept
{
    if (this != &other) {
        Clear();

        device_info_ = std::move(other.device_info_);
        descriptor_pools_ = std::move(other.descriptor_pools_);

        global_graphics_descsets_ = std::move(other.global_graphics_descsets_);

        pipelines_ = std::move(other.pipelines_);
        descriptor_set_layouts_ = std::move(other.descriptor_set_layouts_);
        bindings_ = std::move(other.bindings_);

        pipeline_pbindings_ = std::move(other.pipeline_pbindings_);

        width_ = std::move(other.width_);
        height_ = std::move(other.height_);

        other.device_info_ = {};
        other.global_graphics_descsets_ = {};
    }

    return *this;
}

void WVkGraphicsPipelines::CreateRenderPipeline(
    const WAssetId & in_id,
    const WRenderPipelineStruct & pipeline_struct
    ) {

    WVkRenderPipelineInfo render_pipeline_info;
    render_pipeline_info.type = pipeline_struct.type;

    std::vector<WVkShaderStageInfo> shaders;
    shaders.reserve(pipeline_struct.shaders_count);

    // TODO transition to slang
    for (uint8_t i=0; i < pipeline_struct.shaders_count; i++) {
        shaders.push_back(
            WVulkan::BuildGraphicsShaderStageInfo(
                WStringUtils::SystemPath(pipeline_struct.shaders[i].file).c_str(),
                pipeline_struct.shaders[i].entry,
                pipeline_struct.shaders[i].type
                )
            );
    }
    
    // Use asset pipeline id too.
    CreateGraphicDescriptorSetLayout(in_id);

    auto& d_set_layout = descriptor_set_layouts_.Get(in_id);

    WVulkan::Create(
        render_pipeline_info,
        device_info_,
        {
            global_graphics_descsets_.descset_layout_info,
            d_set_layout
        },
        shaders
        );

    pipelines_.InsertAt(in_id, render_pipeline_info);
            
    render_pipeline_info.wid = in_id;
    render_pipeline_info.descriptor_set_layout_id = in_id;

    for(std::uint32_t i=0; i<WENG_MAX_FRAMES_IN_FLIGHT; i++) {
        WVkDescriptorPoolInfo dpoolinfo{};

        // Create a descriptor pool by frame index
        WDescPoolUtils::CreateGraphicsDescSetPool(
            dpoolinfo, device_info_
            );

        descriptor_pools_[i].InsertAt(
            in_id, dpoolinfo
            );
    }

    pipeline_pbindings_[in_id] = {};
}

void WVkGraphicsPipelines::DeleteRenderPipeline(
    const WAssetId & in_id
    )
{
    // Remove bindings
    for (auto & bid : pipeline_pbindings_[in_id]) {
        bindings_.Remove(bid) ;
    }

    pipeline_pbindings_.erase(in_id);
    
    pipelines_.Remove(in_id);

    for (std::uint8_t i=0; i<WENG_MAX_FRAMES_IN_FLIGHT; i++) {
        descriptor_pools_[i].Remove(in_id);
    }
}

void WVkGraphicsPipelines::ResetDescriptorPool(const WAssetId & in_pipeline_id,
                                                    const std::uint32_t & in_frameindex) {
    vkResetDescriptorPool(
        device_info_.vk_device,
        descriptor_pools_[in_frameindex].Get(in_pipeline_id).descriptor_pool,
        0
        );
}

WId WVkGraphicsPipelines::CreateBinding(
    const WEntityComponentId & component_id,
    const WAssetId & in_pipeline_id,
    const WAssetIndexId & in_mesh_asset_id,
    std::vector<WVkTextureInfo> in_textures,
    std::vector<uint16_t> in_textures_bindings
    ) noexcept
{
    assert(pipeline_pbindings_.contains(in_pipeline_id));

    WVkRenderPipelineInfo pipeline_info = RenderPipelineInfo(in_pipeline_id);

    // Lambda ensures NRVO and avoids moves
    //  Create Descriptor binding objects with default values
    auto f = [this] () {
        std::array<WVkDescriptorSetUBOBinding, WENG_MAX_FRAMES_IN_FLIGHT> bindings;

        for(uint32_t i = 0; i < bindings.size(); i++) {
            
            bindings[i].binding = 0;
            bindings[i].ubo_info.range = sizeof(WUBOModelStruct);

            WVulkan::CreateUBO(bindings[i].ubo_info, device_info_);

            bindings[i].buffer_info.buffer = bindings[i].ubo_info.uniform_buffer;
            bindings[i].buffer_info.offset = 0;
            bindings[i].buffer_info.range = bindings[i].ubo_info.range;
        }

        return bindings;
    };

    auto t = [this,
              &in_textures,
              &in_textures_bindings] () {
        std::vector<WVkDescriptorSetTextureBinding> tx{in_textures.size()};
        
        for (uint32_t i=0; i<WENG_MAX_FRAMES_IN_FLIGHT; i++) {
            for (uint32_t j = 0; j<tx.size(); j++) {

                tx[j].image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                tx[j].binding = in_textures_bindings[j];
                tx[j].image_info.imageView = in_textures[j].image_view;
                tx[j].image_info.sampler = in_textures[j].sampler;
            }
        }
        return tx;
    };

    bindings_.InsertAt(
        component_id,
        WVkPipelineBindingInfo{in_pipeline_id,
                               in_mesh_asset_id,
                               t(),
                               f()}
        );

    pipeline_pbindings_[in_pipeline_id].Insert(component_id.GetId(), component_id);

    return component_id;
}

void WVkGraphicsPipelines::DeleteBinding(const WEntityComponentId & in_id) {
    bindings_.Remove(in_id);

    for(auto & p : pipeline_pbindings_) {
        if (p.second.Contains(in_id.GetId())) {
            p.second.Remove(in_id.GetId());            
        }
    }
}

void WVkGraphicsPipelines::Clear()
{
    bindings_.Clear();
    pipelines_.Clear();

    for(std::uint32_t i=0; i<WENG_MAX_FRAMES_IN_FLIGHT; i++) {
        descriptor_pools_[i].Clear();
    }

    descriptor_set_layouts_.Clear();
}

void WVkGraphicsPipelines::Destroy() {
    bindings_.Clear();
    pipelines_.Clear();

    for(std::uint32_t i=0; i<WENG_MAX_FRAMES_IN_FLIGHT; i++) {
        descriptor_pools_[i].Clear();
    }

    descriptor_set_layouts_.Clear();

    Destroy_GlobalGraphics();

    device_info_ = {};
 
    width_=0;
    height_=0;
}

void WVkGraphicsPipelines::CreateGraphicDescriptorSetLayout(const WAssetId & in_id) {

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

void WVkGraphicsPipelines::Initialize_ClearLambdas() {

    bindings_.SetDestroyFn([di_=device_info_](auto & b) {
        for(auto& ubo: b.ubo) {
            WVulkan::Destroy(ubo.ubo_info, di_);
        }
    });

    for(std::uint32_t i=0; i < WENG_MAX_FRAMES_IN_FLIGHT; i++) {
        descriptor_pools_[i].SetDestroyFn([di_=device_info_](auto & b) {
            WDescPoolUtils::Destroy(b, di_);
        });
    }

    pipelines_.SetDestroyFn([di_=device_info_](auto & p) {
        WVulkan::Destroy(
            p,
            di_
            );
    });

    descriptor_set_layouts_.SetDestroyFn([di_=device_info_](auto & d) {
        WVulkan::Destroy(
            d,
            di_
            );
    });
}

void WVkGraphicsPipelines::Initialize_GlobalGraphicDescriptors() {

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

    for (std::uint32_t i=0; i<WENG_MAX_FRAMES_IN_FLIGHT; i++) {
        WVulkan::Create(
            global_graphics_descsets_.descset_info[i],
            device_info_,
            global_graphics_descsets_.descset_layout_info,
            global_graphics_descsets_.descpool_info
            );        
    }

    std::array<VkWriteDescriptorSet, WENG_MAX_FRAMES_IN_FLIGHT> ws;

    for(uint32_t i=0; i < WENG_MAX_FRAMES_IN_FLIGHT; i++) {
        global_graphics_descsets_.camera_ubo[i].range = sizeof(WUBOCameraStruct);

        WVulkan::CreateUBO(
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
            global_graphics_descsets_.descset_info[i].descriptor_set
            );
    }

    WVulkan::UpdateDescriptorSets(
        ws,
        device_info_
        );

}

void WVkGraphicsPipelines::Destroy_GlobalGraphics() {

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

    global_graphics_descsets_ = GlobalGraphicsResources();
}

void WVkGraphicsPipelines::UpdateGlobalGraphicsDescriptorSet(
    const WUBOCameraStruct & camera_struct,
    uint32_t in_frame_index
    ) {
    WVulkan::MapUBO(
        global_graphics_descsets_.camera_ubo[in_frame_index],
        device_info_
        );
    
    memcpy(
        global_graphics_descsets_.camera_ubo[in_frame_index].mapped_data,
        &camera_struct,
        sizeof(WUBOCameraStruct)
        );
    
    WVulkan::UnmapUBO(
        global_graphics_descsets_.camera_ubo[in_frame_index],
        device_info_
        );
}

void WVkGraphicsPipelines::UpdateModelDescriptorSet(
    const WUBOModelStruct & in_ubo_model_struct,
    const WEntityComponentId & in_desc_set,
    uint32_t in_frame_index
    ) {
    WVulkan::MapUBO(
        bindings_.Get(in_desc_set).ubo[in_frame_index].ubo_info,
        device_info_
        );
    WVulkan::UpdateUBOModel(
        bindings_.Get(in_desc_set).ubo[in_frame_index].ubo_info,
        // Initial Position
        in_ubo_model_struct
        );
    WVulkan::UnmapUBO(
        bindings_.Get(in_desc_set).ubo[in_frame_index].ubo_info,
        device_info_
        );
}

void WVkGraphicsPipelines::UpdateModelDescriptorSet(
    const WUBOModelStruct & in_ubo_model_struct,
    const WEntityComponentId & in_desc_set
    ) {
    for(int i=0; i<WENG_MAX_FRAMES_IN_FLIGHT; i++) {
        UpdateModelDescriptorSet(
            in_ubo_model_struct,
            in_desc_set,
            i
            );
    }
}



