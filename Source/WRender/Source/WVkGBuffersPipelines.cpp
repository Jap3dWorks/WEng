#include "WVulkan/WVkGBuffersPipelines.hpp"
#include "WStructs/WRenderStructs.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WLog.hpp"
#include "WVkGBuffersPipelinesUtils.hpp"
#include "WVulkan/WVulkanUtils.hpp"

#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cassert>
#include <utility>

// WRenderPipelinesManager
// -------------------

WVkGBuffersPipelines::~WVkGBuffersPipelines()
{
    Destroy();
}

WVkGBuffersPipelines::WVkGBuffersPipelines(
    WVkGBuffersPipelines && other
    ) noexcept :
    Super(std::move(other)),
    global_graphics_descsets_(std::move(other.global_graphics_descsets_))
{
    other.global_graphics_descsets_ = {};
}

WVkGBuffersPipelines & WVkGBuffersPipelines::operator=(WVkGBuffersPipelines && other) noexcept
{
    if (this != &other) {
        Super::operator=(std::move(other));

        global_graphics_descsets_ = std::move(other.global_graphics_descsets_);

        other.global_graphics_descsets_ = {};
    }

    return *this;
}

void WVkGBuffersPipelines::CreatePipeline(
    const WAssetId & in_id,
    const WRenderPipelineStruct & in_pipeline_struct
    ) {

    std::vector<WVkShaderStageInfo> shaders = pipelines_db_.BuildShaders(
        in_pipeline_struct.shaders,
        WVkGBuffersPipelinesUtils::BuildShaderStageInfo
        );

    // pipeparams
    // one Ubo should be required as first binding (0)
    pipelines_db_.CreateDescSetLayout(
        in_id,
        device_,
        in_pipeline_struct.params_descriptor,
        WVulkanUtils::UpdateDescriptorSetLayout
        );

    pipelines_db_.CreatePipeline(
        in_id,
        device_,
        in_id,
        shaders,
        [this, &in_pipeline_struct]
        (auto& _rp, const auto &_dvc, const auto &_desclay, const auto & _shdrs) {

            WVkGBuffersPipelinesUtils::CreatePipeline(
                _rp,
                _dvc,
                {
                    global_graphics_descsets_.descset_layout_info,
                    _desclay
                },
                _shdrs
                );
            _rp.params_descriptor = in_pipeline_struct.params_descriptor;
        }
        );

    pipelines_db_.CreateDescSetPool(
        in_id,
        device_,
        WVkGBuffersPipelinesUtils::CreateDescSetPool);

    pipeline_bindings_[in_id] = {};
}

WId WVkGBuffersPipelines::CreateBinding(
    const WEntityComponentId & component_id,
    const WAssetId & in_pipeline_id,
    const WAssetIndexId & in_mesh_asset_id,
    const std::vector<WVkDescriptorSetUBOWriteStruct> & in_ubos,
    const std::vector<WVkDescriptorSetTextureWriteStruct> & in_textures
    )
{
    WVkRenderPipelineInfo pipeline_info = Pipeline(in_pipeline_id);

    pipelines_db_.bindings.InsertAt(
        component_id,
        WVkPipelineBindingInfo{in_pipeline_id,
                               in_mesh_asset_id,
                               InitUboDescriptorBindings(pipeline_info.params_descriptor,
                                                         in_ubos),
                               InitTextureDescriptorBindings(pipeline_info.params_descriptor,
                                                             in_textures)}
        );

    pipeline_bindings_[in_pipeline_id].Insert(component_id.GetId(), component_id);

    return component_id;
}

void WVkGBuffersPipelines::Destroy() {

    ClearPipelinesDb();
    
    Destroy_GlobalResources();

    device_ = VK_NULL_HANDLE;
}

void WVkGBuffersPipelines::Initialize_GlobalResources() {
    
    WVkGBuffersPipelinesUtils::UpdateDSL_DefaultGlobalGraphicBindings(
        global_graphics_descsets_.descset_layout_info
        );

    WVulkan::Create(
        global_graphics_descsets_.descset_layout_info,
        device_
        );

    // TODO CreateGlobalDescPool (camera and lights)
    WVulkan::Create(
        global_graphics_descsets_.descpool_info,
        device_
        );

    for (std::uint32_t i=0; i<frames_in_flight; i++) {
        WVulkan::Create(
            global_graphics_descsets_.descset_info[i],
            device_,
            global_graphics_descsets_.descset_layout_info,
            global_graphics_descsets_.descpool_info
            );        
    }

    for(uint32_t i=0; i < frames_in_flight; i++) {
        
        global_graphics_descsets_.camera_ubo[i].range = sizeof(WUBOCameraStruct);

        WVulkan::CreateUBO(
            global_graphics_descsets_.camera_ubo[i],
            device_,
            physical_device_
            );

        VkWriteDescriptorSet ws = {};
        VkDescriptorBufferInfo buffer_info{};

        buffer_info.buffer = global_graphics_descsets_.camera_ubo[i].buffer;
        buffer_info.offset = 0;
        buffer_info.range = global_graphics_descsets_.camera_ubo[i].range;

        WVulkan::UpdateWriteDescriptorSet_UBO(
            ws,
            0,
            buffer_info,
            global_graphics_descsets_.descset_info[i].descriptor_set
            );

        vkUpdateDescriptorSets(
            device_,
            1,
            &ws,
            0,
            nullptr
            );
    }
}

void WVkGBuffersPipelines::Destroy_GlobalResources() {

    if (device_) {

        WVulkan::Destroy(global_graphics_descsets_.descpool_info,
                         device_);

        for(uint32_t i=0; i<global_graphics_descsets_.camera_ubo.size(); i++) {
            WVulkan::Destroy(
                global_graphics_descsets_.camera_ubo[i],
                device_);
        }

        if (global_graphics_descsets_
            .descset_layout_info
            .descset_layout)
        {
            WVulkan::Destroy(
                global_graphics_descsets_.descset_layout_info,
                device_
                );
        }
    }

    global_graphics_descsets_ = GlobalGraphicsResources();
}

void WVkGBuffersPipelines::UpdateGlobalGraphicsDescriptorSet(
    const WUBOCameraStruct & camera_struct,
    uint32_t in_frame_index
    ) {
    WVulkan::MapUBO(
        global_graphics_descsets_.camera_ubo[in_frame_index],
        device_
        );
    
    memcpy(
        global_graphics_descsets_.camera_ubo[in_frame_index].mapped_memory,
        &camera_struct,
        sizeof(WUBOCameraStruct)
        );
    
    WVulkan::UnmapUBO(
        global_graphics_descsets_.camera_ubo[in_frame_index],
        device_
        );
}


