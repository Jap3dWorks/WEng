#include "WVulkan/WVkGraphicsPipelines.hpp"
#include "WStructs/WRenderStructs.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WLog.hpp"
#include "WVkGraphicsPipelinesUtils.hpp"

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
    WVkDeviceInfo device
    ) : Super(device)
{
    WFLOG("Initialize Global Graphic Descriptors.");
    Initialize_GlobalGraphicDescriptors();
}

WVkGraphicsPipelines::WVkGraphicsPipelines(
    WVkGraphicsPipelines && other
    ) noexcept :
    Super(std::move(other)),
    global_graphics_descsets_(std::move(other.global_graphics_descsets_))
{
    other.global_graphics_descsets_ = {};
}

WVkGraphicsPipelines & WVkGraphicsPipelines::operator=(WVkGraphicsPipelines && other) noexcept
{
    if (this != &other) {
        Super::operator=(std::move(other));

        global_graphics_descsets_ = std::move(other.global_graphics_descsets_);

        other.global_graphics_descsets_ = {};
    }

    return *this;
}

void WVkGraphicsPipelines::CreatePipeline(
    const WAssetId & in_id,
    const WRenderPipelineStruct & pipeline_struct
    ) {

    std::vector<WVkShaderStageInfo> shaders = pipelines_db_.BuildShaders(
        pipeline_struct.shaders_count,
        pipeline_struct.shaders,
        WVkGraphicsPipelinesUtils::BuildGraphicsShaderStageInfo
        );

    pipelines_db_.CreateDescSetLayout(
        in_id,
        device_info_,
        WVkGraphicsPipelinesUtils::UpdateDSL_DefaultGraphicBindings
        );

    pipelines_db_.CreatePipeline(
        in_id,
        device_info_,
        in_id,
        shaders,
        [this](auto& _rp, const auto &_dvc, const auto &_desclay, const auto & _shdrs) {
            WVulkan::CreateDefaultPipeline(
                _rp,
                _dvc,
                {
                    global_graphics_descsets_.descset_layout_info,
                    _desclay
                },
                _shdrs
                );
        }
        );

    pipelines_db_.CreateDescSetPool(
        in_id,
        device_info_,
        WVkGraphicsPipelinesUtils::CreateGraphicsDescSetPool);

    pipeline_pbindings_[in_id] = {};
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

    WVkRenderPipelineInfo pipeline_info = Pipeline(in_pipeline_id);

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

    pipelines_db_.bindings.InsertAt(
        component_id,
        WVkPipelineBindingInfo{in_pipeline_id,
                               in_mesh_asset_id,
                               t(),
                               f()}
        );

    pipeline_pbindings_[in_pipeline_id].Insert(component_id.GetId(), component_id);

    return component_id;
}

void WVkGraphicsPipelines::Destroy() {

    ClearPipelinesDb();
    
    Destroy_GlobalGraphics();

    device_info_ = {};
}

void WVkGraphicsPipelines::Initialize_GlobalGraphicDescriptors() {

    WVkGraphicsPipelinesUtils::UpdateDSL_DefaultGlobalGraphicBindings(
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
    const WEntityComponentId & in_binding_id,
    uint32_t in_frame_index
    ) {
    auto & binding = pipelines_db_.bindings.Get(in_binding_id);

    WVulkan::MapUBO(
        binding.ubo[in_frame_index].ubo_info,
        device_info_
        );
    WVulkan::UpdateUBOModel(
        binding.ubo[in_frame_index].ubo_info,
        in_ubo_model_struct
        );
    WVulkan::UnmapUBO(
        binding.ubo[in_frame_index].ubo_info,
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



