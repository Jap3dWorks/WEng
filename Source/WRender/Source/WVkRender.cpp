#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

#include "WVulkan/WVkRender.hpp"
#include "WRenderUtils.hpp"
#include "WCore/WCore.hpp"
#include "WAssets/WRenderPipelineAsset.hpp"
#include "WVulkan/WVulkan.hpp"
#include "WVulkan/WVkRenderCommandPool.hpp"
#include "WVulkan/WVkRenderConfig.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVkGraphicsPipelines.hpp"
#include "WVulkan/WVkAssetResources.hpp"
#include "WStructs/WComponentStructs.hpp"
#include "WStructs/WRenderStructs.hpp"
#include "WVulkan/WVkRenderUtils.hpp"
#include "WLog.hpp"

#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include <array>

#ifdef NDEBUG
    #define _ENABLE_VALIDATON_LAYERS true
#else
    #define _ENABLE_VALIDATON_LAYERS true
#endif

// WVkRender
// -------

WVkRender::WVkRender() noexcept :
    instance_info_(),
    window_(),
    render_size_(),
    surface_info_(),
    device_info_(),
    debug_info_(),
    render_resources_(),
    swap_chain_info_(),
    swap_chain_resources_(),
    offscreen_render_(),
    postprocess_render_(),
    render_command_pool_(),
    render_command_buffer_(),
    graphics_pipelines_(),
    sync_semaphores_(),
    semaphore_index_(0),
    sync_fences_(),
    frame_index_(0)
{
}

WVkRender::WVkRender(WVkRender && other) noexcept :
    instance_info_(std::move(other.instance_info_)),
    window_(std::move(other.window_)),
    render_size_(std::move(other.render_size_)),
    surface_info_(std::move(other.surface_info_)),
    device_info_(std::move(other.device_info_)),
    debug_info_(std::move(other.debug_info_)),
    render_resources_(std::move(other.render_resources_)),
    swap_chain_info_(std::move(other.swap_chain_info_)),
    swap_chain_resources_(std::move(other.swap_chain_resources_)),
    offscreen_render_(std::move(other.offscreen_render_)),
    postprocess_render_(std::move(other.postprocess_render_)),
    render_command_pool_(std::move(other.render_command_pool_)),
    render_command_buffer_(std::move(other.render_command_buffer_)),
    graphics_pipelines_(std::move(other.graphics_pipelines_)),
    sync_semaphores_(std::move(other.sync_semaphores_)),
    semaphore_index_(std::move(other.semaphore_index_)),
    sync_fences_(std::move(other.sync_fences_)),
    frame_index_(std::move(other.frame_index_))
{
}

WVkRender & WVkRender::operator=(WVkRender && other) noexcept {
    if (this != &other) {
        instance_info_ = std::move(other.instance_info_);
        window_ = std::move(other.window_);
        render_size_ = std::move(other.render_size_);
        surface_info_ = std::move(other.surface_info_);
        device_info_ = std::move(other.device_info_);
        debug_info_ = std::move(other.debug_info_);
        render_resources_ = std::move(other.render_resources_);
        swap_chain_info_ = std::move(other.swap_chain_info_);
        swap_chain_resources_ = std::move(other.swap_chain_resources_);
        offscreen_render_ = std::move(other.offscreen_render_);
        postprocess_render_ = std::move(other.postprocess_render_);
        render_command_pool_ = std::move(other.render_command_pool_);
        render_command_buffer_ = std::move(other.render_command_buffer_);
        graphics_pipelines_ = std::move(other.graphics_pipelines_);
        sync_semaphores_ = std::move(other.sync_semaphores_);
        semaphore_index_ = std::move(other.semaphore_index_);
        sync_fences_ = std::move(other.sync_fences_);
        frame_index_ = std::move(other.frame_index_);
    }

    return *this;
}

WVkRender::WVkRender(GLFWwindow * in_window) : WVkRender() {
    Window(in_window);
}

WVkRender::~WVkRender()
{
    if (device_info_.vk_device) {
        Destroy();
    }
}

void WVkRender::WaitIdle() const
{
    vkDeviceWaitIdle(device_info_.vk_device);
}

void WVkRender::Window(GLFWwindow * in_window) {
    window_.window = in_window;
    std::int32_t width, height;
    
    glfwGetFramebufferSize(window_.window, &width, &height);

    // window_.width = static_cast<std::uint32_t>(width);
    // window_.height = static_cast<std::uint32_t>(height);

    render_size_.width = static_cast<std::uint32_t>(width);
    render_size_.height = static_cast<std::uint32_t>(height);
}

void WVkRender::Initialize()
{
    debug_info_.enable_validation_layers = _ENABLE_VALIDATON_LAYERS;

    std::array<std::uint32_t,2> dimensions = {
        render_size_.width,
        render_size_.height
    };

    // Create Vulkan Instance
    WVulkan::Create(
        instance_info_,
        debug_info_
        );

    // Create Vulkan Window Surface
    WVulkan::Create(
        surface_info_,
        instance_info_, 
        window_.window
        );

    // Create Vulkan Device
    WVulkan::Create(
        device_info_,
        instance_info_,
        surface_info_,
        debug_info_
        );

    // Create Vulkan Swap Chain
    WVulkan::Create(
        swap_chain_info_,
        device_info_,
        surface_info_,
        dimensions[0],
        dimensions[1],
        debug_info_
        );

    // Offscreen Render Pass

    WVkRenderUtils::CreateOffscreenRender(
        offscreen_render_,
        device_info_,
        dimensions[0],
        dimensions[1],
        swap_chain_info_.format
        );
    
    // Postprocess Render Pass

    WVkRenderUtils::CreatePostprocessRender(
        postprocess_render_,
        device_info_,
        dimensions[0],
        dimensions[1],
        swap_chain_info_.format
        );

    // --

    render_command_pool_ = WVkRenderCommandPool( 
        WVkCommandPoolInfo(),
        device_info_,
        surface_info_
        );

    graphics_pipelines_.Initialize(device_info_);
    
    ppcss_pipelines_.Initialize(
        device_info_,
        render_command_pool_.CommandPoolInfo()
        );

    render_command_buffer_ =
        render_command_pool_.
        CreateCommandBuffer();

    sync_semaphores_ = WVkRenderUtils::CreateSyncSemaphore<SyncSemaphores>(
        swap_chain_info_.images.size(),
        device_info_.vk_device
        );

    sync_fences_ = WVkRenderUtils::CreateSyncFences<WENG_MAX_FRAMES_IN_FLIGHT>(
        device_info_.vk_device
        );

    render_resources_ = WVkAssetResources(
        device_info_,
        render_command_pool_.CommandPoolInfo()
        );

    swap_chain_resources_.Initialize(
        device_info_,
        render_command_pool_.CommandPoolInfo()
        );

}

void WVkRender::Draw()
{

    vkWaitForFences(
        device_info_.vk_device,
        1,
        &sync_fences_[frame_index_],
        VK_TRUE,
        UINT64_MAX
        );
    
    uint32_t image_index;

    VkResult result = vkAcquireNextImageKHR(
        device_info_.vk_device,
        swap_chain_info_.swap_chain,
        UINT64_MAX,
        sync_semaphores_[semaphore_index_].image_available,
        VK_NULL_HANDLE,
        &image_index
        );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquiera swap chain image!");
    }

    vkResetFences(
        device_info_.vk_device,
        1,
        &sync_fences_[frame_index_]
        );

    // Begin command buffer

    WVkRenderUtils::BeginRenderCommandBuffer(
        render_command_buffer_.command_buffers[frame_index_]
        );

    RecordOffscreenRenderCommandBuffer(
        render_command_buffer_.command_buffers[frame_index_],
        frame_index_);

    RecordPostprocessRenderCommandBuffer(
        render_command_buffer_.command_buffers[frame_index_],
        frame_index_,
        image_index
        );

    // End Command buffer

    WVkRenderUtils::EndRenderCommandBuffer(
        render_command_buffer_.command_buffers[frame_index_]
        );

    VkSubmitInfo submit_info;
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext = nullptr;

    VkPipelineStageFlags wait_stages[] =
        { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &sync_semaphores_[semaphore_index_].image_available;
    submit_info.pWaitDstStageMask = wait_stages;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers =
        &render_command_buffer_.command_buffers[frame_index_];

    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &sync_semaphores_[image_index].render_finished;

    if (vkQueueSubmit(
            device_info_.vk_graphics_queue,
            1,
            &submit_info,
            sync_fences_[frame_index_]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit draw command buffer");
    }

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &sync_semaphores_[image_index].render_finished;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swap_chain_info_.swap_chain;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr;

    result = vkQueuePresentKHR(device_info_.vk_present_queue,
                               &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        RecreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    semaphore_index_ = (semaphore_index_ + 1) % sync_semaphores_.size();
    frame_index_ = (frame_index_ + 1) % WENG_MAX_FRAMES_IN_FLIGHT;
}

// Pipelines

void WVkRender::CreateRenderPipeline(
    WRenderPipelineAsset * render_pipeline
    ) {

    switch(render_pipeline->RenderPipeline().type) {
        
    case EPipelineType::Graphics:
        graphics_pipelines_.CreatePipeline(
            render_pipeline->WID(),
            render_pipeline->RenderPipeline()
            );
        break;
        
    case EPipelineType::Postprocess:
        ppcss_pipelines_.CreatePipeline(
            render_pipeline->WID(),
            render_pipeline->RenderPipeline()
            );
        break;

    default:
        WFLOG("Pipeline type not implemented.");
    }

    pipeline_track_.pipeline_ptype[render_pipeline->WID()] =
        render_pipeline->RenderPipeline().type;

}

void WVkRender::DeleteRenderPipeline(const WAssetId & in_id) {

    auto clearbindingfn = [this](const WEntityComponentId & binding) {
        pipeline_track_.binding_ptype.erase(binding);
    };
        
    switch(pipeline_track_.pipeline_ptype[in_id]) {
        
    case EPipelineType::Graphics:
        graphics_pipelines_.ForEachBinding(
            in_id, clearbindingfn
            );
            
        graphics_pipelines_.DeletePipeline(
            in_id
            );
            
        break;

    case EPipelineType::Postprocess:
        ppcss_pipelines_.ForEachBinding(
            in_id, clearbindingfn
            );

        graphics_pipelines_.DeletePipeline(
            in_id
            );
        
        break;

    default:
        WFLOG("Pipeline type not implemented.");
    }

    pipeline_track_.pipeline_ptype.erase(in_id);

}

void WVkRender::CreatePipelineBinding(
    const WEntityComponentId & component_id,
    const WAssetId & pipeline_id,
    const WAssetIndexId & in_assetindex_id,
    const WRenderPipelineParametersStruct & in_parameters
    )
{
    assert(pipeline_track_.pipeline_ptype.contains(pipeline_id));

    std::vector<WVkDescriptorSetTextureWriteStruct> textures{};
    textures.resize(in_parameters.texture_params.size());

    for(std::uint32_t i=0; i < in_parameters.texture_params.size(); i++) {
        const auto & tx = render_resources_.TextureInfo(
            in_parameters.texture_params[i].value
            );
        
        textures[i] = {
            .binding = in_parameters.texture_params[i].binding,
            .image_info = {
                .sampler = tx.sampler,
                .imageView = tx.view,
                .imageLayout = tx.layout
            }
        };
    }

    std::vector<WVkDescriptorSetUBOWriteStruct> ubos{};
    ubos.resize(in_parameters.ubo_params.size());

    for(std::uint32_t i=0; i < in_parameters.ubo_params.size(); i++) {
        const auto & ubop = in_parameters.ubo_params[i];
        ubos[i] = {
            .binding = ubop.binding,
            .data = ubop.databuffer.data(),
            .size = ubop.databuffer.size(),
            .offset = ubop.offset
        };
    }

    switch(pipeline_track_.pipeline_ptype[pipeline_id]) {

    case EPipelineType::Graphics:
        graphics_pipelines_.CreateBinding(component_id,
                                          pipeline_id,
                                          in_assetindex_id,
                                          ubos,
                                          textures);
        break;
        
    case EPipelineType::Postprocess:
        ppcss_pipelines_.CreateBinding(component_id,
                                       pipeline_id,
                                       ubos,
                                       textures);
        break;
        
    default:
        WFLOG("Pipeline type not implemented.");
    }

    pipeline_track_.binding_ptype[component_id] =
        pipeline_track_.pipeline_ptype[pipeline_id];
    
}

void WVkRender::DeletePipelineBinding(const WEntityComponentId & in_id) {

    switch(pipeline_track_.binding_ptype[in_id]) {
        
    case EPipelineType::Graphics:
        graphics_pipelines_.DeleteBinding(in_id);
        break;
        
    case EPipelineType::Postprocess:
        ppcss_pipelines_.DeleteBinding(in_id);
        break;
        
    default:
        WFLOG("Pipeline type not implemented.");
    }

    pipeline_track_.binding_ptype.erase(in_id);
}

void WVkRender::RefreshPipelines() {
    ppcss_pipelines_.CalcBindingOrder();
}

void WVkRender::ClearPipelines() {
    // preserve global pipelines
    graphics_pipelines_.ClearPipelinesDb();
    ppcss_pipelines_.ClearPipelinesDb();
    ppcss_pipelines_.CalcBindingOrder();
}

// Resources
// ---------

void WVkRender::UnloadAllResources() {
    render_resources_.Clear();
}

void WVkRender::UpdateUboCamera(
    const WUBOCameraStruct & camera_ubo
    ) {
    graphics_pipelines_.UpdateGlobalGraphicsDescriptorSet(
        camera_ubo,
        frame_index_
        );
}

void WVkRender::UpdateParameterDynamic(
        const WEntityComponentId & in_component_id,
        const WRPParamUboStruct & ubo_write
    ) {

    WVkDescriptorSetUBOWriteStruct ubowrt{
        .binding = ubo_write.binding,
        .data = ubo_write.databuffer.data(),
        .size = ubo_write.databuffer.size(),
        .offset = ubo_write.offset
    };

    switch(pipeline_track_.binding_ptype[in_component_id]) {

    case EPipelineType::Graphics:
        graphics_pipelines_.UpdateBinding(in_component_id,
                                          frame_index_,
                                          ubowrt);
        break;

    case EPipelineType::Postprocess:
        ppcss_pipelines_.UpdateBinding(in_component_id,
                                       frame_index_,
                                       ubowrt);
        break;

    default:
        WFLOG("Pipeline type not implemented.");
    }
}

void WVkRender::UpdateParameterStatic(
        const WEntityComponentId & in_component_id,
        const WRPParamUboStruct & ubo_write
    ) {

    WVkDescriptorSetUBOWriteStruct ubowrt{
        .binding = ubo_write.binding,
        .data = ubo_write.databuffer.data(),
        .size = ubo_write.databuffer.size(),
        .offset = ubo_write.offset
    };

    switch(pipeline_track_.binding_ptype[in_component_id]) {
        
    case EPipelineType::Graphics:
        graphics_pipelines_.UpdateBinding(
            in_component_id, ubowrt
            );
        break;

    case EPipelineType::Postprocess:
        ppcss_pipelines_.UpdateBinding(in_component_id,
                                       ubowrt);
        break;

    default:
        WFLOG("Pipeline type not implemented.");
    }
}

void WVkRender::Destroy() {

    WFLOG("Destroy WVkRender...");

    WFLOG("Destroy Render Pipelines.");
    graphics_pipelines_.Destroy();
    ppcss_pipelines_.Destroy();

    WFLOG("Destroy Fences and Semaphores.");

    WVkRenderUtils::DestroySyncSemaphores(
        sync_semaphores_,
        device_info_.vk_device
        );

    WVkRenderUtils::DestroySyncFences(
        sync_fences_,
        device_info_.vk_device
        );
    
    WVkRenderUtils::DestroyOffscreenRender(
        offscreen_render_,
        device_info_
        );

    WVkRenderUtils::DestroyPostprocessRender(
        postprocess_render_,
        device_info_
        );

    WFLOG("Destroy Swap Chain Info.");

    // Destroy Swap Chain and Image Views
    WVulkan::Destroy(swap_chain_info_, device_info_);

    swap_chain_resources_.Destroy();

    WFLOG("Destroy Render Command Pool.");

    render_command_pool_.Clear();
    render_command_pool_ = {};
    render_command_buffer_ = {};

    WFLOG("Destroy Render Resources.");

    render_resources_.Clear();

    WFLOG("Destroy Vulkan Device.");

    // Destroy Vulkan Device
    WVulkan::Destroy(device_info_);

    // Destroy Vulkan Surface
    WVulkan::Destroy(surface_info_, instance_info_);

    // Destroy Vulkan Instance
    WVulkan::Destroy(instance_info_);

}

void WVkRender::Rescale(const std::uint32_t & in_width, const std::uint32_t & in_height) {

    render_size_.width = in_width;
    render_size_.height = in_height;

    RecreateSwapChain();
}

void WVkRender::RecreateSwapChain() {
    WFLOG("RECREATE SWAP CHAIN!");

    std::array<std::uint32_t,2> dimensions = {
        render_size_.width,
        render_size_.height
    };

    WaitIdle();

    WVulkan::Destroy(
        swap_chain_info_,
        device_info_
        );

    WVkRenderUtils::DestroyOffscreenRender(
        offscreen_render_,
        device_info_
        );

    WVkRenderUtils::DestroyPostprocessRender(
        postprocess_render_,
        device_info_
        );

    WVulkan::Create(
        swap_chain_info_,
        device_info_,
        surface_info_,
        dimensions[0],
        dimensions[1],
        debug_info_
        );

    WVkRenderUtils::CreateOffscreenRender(
        offscreen_render_,
        device_info_,
        dimensions[0],
        dimensions[1],
        swap_chain_info_.format
        );

    WVkRenderUtils::CreatePostprocessRender(
        postprocess_render_,
        device_info_,
        dimensions[0],
        dimensions[1],
        swap_chain_info_.format
        );
}

void WVkRender::RecordOffscreenRenderCommandBuffer(
    const VkCommandBuffer & in_command_buffer,
    const std::uint32_t & in_frame_index
    )
{
    // Image Layouts
    WVkRenderUtils::RndCmd_TransitionRenderImageLayout(
        in_command_buffer,
        offscreen_render_[in_frame_index].color.image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        {},
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        );

    WVkRenderUtils::RndCmd_TransitionRenderImageLayout(
        in_command_buffer,
        offscreen_render_[in_frame_index].depth.image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        {},
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT
        );

    WVkRenderUtils::RndCmd_BeginOffscreenRendering(
        in_command_buffer,
        offscreen_render_[in_frame_index].color.view,
        offscreen_render_[in_frame_index].depth.view,
        offscreen_render_[in_frame_index].extent
        );

    for(auto pipeline_id : graphics_pipelines_.IterPipelines()) {
        
        graphics_pipelines_.ResetDescriptorPool(pipeline_id, frame_index_);

        const WVkRenderPipelineInfo & render_pipeline =
            graphics_pipelines_.Pipeline(pipeline_id);

        vkCmdBindPipeline(render_command_buffer_.command_buffers[in_frame_index],
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          render_pipeline.pipeline);

        WVkRenderUtils::RndCmd_SetViewportAndScissor(
            in_command_buffer,
            offscreen_render_[in_frame_index].extent
            );

        for (auto & bid : graphics_pipelines_.IterBindings(pipeline_id)) {

            auto& binding = graphics_pipelines_.Binding(bid);

            // Create descriptor
            VkDescriptorSet descriptorset = WVkRenderUtils::CreateRenderDescriptor(
                device_info_.vk_device,
                graphics_pipelines_.DescriptorPool(pipeline_id, in_frame_index).descriptor_pool,
                graphics_pipelines_.DescriptorSetLayout(pipeline_id).descset_layout,
                frame_index_,
                binding.ubos,
                binding.textures
                );

            auto& mesh_info =
                render_resources_.StaticMeshInfo(
                    binding.mesh_asset_id
                    );

            VkBuffer vertex_buffers[] = {mesh_info.vertex_buffer};
            VkDeviceSize offsets[] = {0};

            vkCmdBindVertexBuffers(
                in_command_buffer,
                0,
                1,
                vertex_buffers,
                offsets
                );

            vkCmdBindIndexBuffer(
                in_command_buffer,
                mesh_info.index_buffer,
                0,
                VK_INDEX_TYPE_UINT32
                );

            std::array<VkDescriptorSet, 2> descsets =
                {
                    graphics_pipelines_.GlobalGraphicsDescriptorSet(frame_index_).descriptor_set,
                    descriptorset
                };

            vkCmdBindDescriptorSets(in_command_buffer,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    render_pipeline.pipeline_layout,
                                    0,
                                    static_cast<std::uint32_t>(descsets.size()),
                                    descsets.data(),
                                    0,
                                    nullptr);

            vkCmdDrawIndexed(in_command_buffer,
                             mesh_info.index_count,
                             1,
                             0,
                             0,
                             0);
        }
    }
    
    vkCmdEndRendering(in_command_buffer);

    // // Transition to postprocess
    WVkRenderUtils::RndCmd_TransitionRenderImageLayout(
        in_command_buffer,
        offscreen_render_[in_frame_index].color.image,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,  // for postprocess
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_SHADER_READ_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
        );
}

void WVkRender::RecordPostprocessRenderCommandBuffer(
    const VkCommandBuffer & in_command_buffer,
    const std::uint32_t & in_frame_index,
    const std::uint32_t & in_image_index
    )
{
    // first input
    
    VkImageView input_view = offscreen_render_[in_frame_index].color.view;
    VkImage input_img = offscreen_render_[in_frame_index].color.image;
    
    VkImageView dst_view = postprocess_render_[in_frame_index].color.view;
    VkImage dst_img = postprocess_render_[in_frame_index].color.image;
    
    std::array<VkImageView, 2> pp_views = {input_view, dst_view};
    std::array<VkImage, 2> pp_images = {input_img, dst_img};

    ppcss_pipelines_.ResetDescriptorPools(in_frame_index);

    std::uint32_t idx=0;
    for(auto pbindingid : ppcss_pipelines_.IterBindingOrder()) {

        WVkPipelineBindingInfo binding = ppcss_pipelines_.Binding(pbindingid);
        WVkRenderPipelineInfo pipeline = ppcss_pipelines_.Pipeline(binding.pipeline_id);
        WVkDescriptorSetLayoutInfo dsetlay = ppcss_pipelines_.DescriptorSetLayout(binding.pipeline_id);
        WVkDescriptorPoolInfo dpool = ppcss_pipelines_.DescriptorPool(binding.pipeline_id, in_frame_index);

        ppcss_pipelines_.ResetGlobalDescriptorPool(in_frame_index);

        // render into layout
        WVkRenderUtils::RndCmd_TransitionRenderImageLayout(
            in_command_buffer,
            dst_img,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            {},
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );

        WVkRenderUtils::RndCmd_BeginPostprocessRendering(
            in_command_buffer,
            dst_view,
            postprocess_render_[in_frame_index].extent
            );

        vkCmdBindPipeline(
            in_command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline.pipeline
            );

        WVkRenderUtils::RndCmd_SetViewportAndScissor(
            in_command_buffer,
            postprocess_render_[in_frame_index].extent
            );

        VkDescriptorSet input_render_descriptor = WVkRenderUtils::CreateInputRenderDescriptor(
            device_info_.vk_device,
            ppcss_pipelines_.GlobalDescriptorPool(in_frame_index).descriptor_pool,
            ppcss_pipelines_.GlobalDescSetLayout().descset_layout,
            input_view,
            ppcss_pipelines_.GlobalSampler()
            );

        VkDescriptorSet pp_descriptor = WVkRenderUtils::CreateRenderDescriptor(
            device_info_.vk_device, dpool.descriptor_pool, dsetlay.descset_layout,
            frame_index_,
            binding.ubos,
            binding.textures
            );

        const WVkMeshInfo& render_plane = ppcss_pipelines_.RenderPlane();

        WVkRenderUtils::RndCmd_PostprocessDrawCommands(
            device_info_.vk_device, in_command_buffer,
            render_plane.vertex_buffer, render_plane.index_buffer,
            render_plane.index_count,
            pipeline.pipeline_layout,
            pipeline.pipeline,
            std::array<VkDescriptorSet,2>{ input_render_descriptor, pp_descriptor }
            );

        vkCmdEndRendering(in_command_buffer);

        idx++;

        // TODO: swap input render
        input_view = pp_views[idx % 2];
        input_img = pp_images[idx % 2];
        dst_view = pp_views[(idx + 1) % 2];
        dst_img = pp_images[(idx + 1) % 2];

        // render from layout
        WVkRenderUtils::RndCmd_TransitionRenderImageLayout(
            in_command_buffer,
            input_img,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );

    }

    VkImage swapchain_image = swap_chain_info_.images[in_image_index];
    VkImageView swapchain_imageview = swap_chain_info_.views[in_image_index];

    // swap chain image layout to render into
    WVkRenderUtils::RndCmd_TransitionRenderImageLayout(
        in_command_buffer,
        swapchain_image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        {},
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        );

    WVkRenderUtils::RndCmd_BeginSwapchainRendering(
        in_command_buffer,
        swapchain_imageview,
        swapchain_imageview,
        swap_chain_info_.extent
        );

    VkDescriptorPool dspool = swap_chain_resources_.DescriptorPool(in_frame_index);
    vkResetDescriptorPool(device_info_.vk_device, dspool, 0);

    VkPipeline pipeline = swap_chain_resources_.Pipeline();
    VkDescriptorSetLayout dslay = swap_chain_resources_.DescriptorSetLayout();

    vkCmdBindPipeline(
        in_command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline
        );

    WVkRenderUtils::RndCmd_SetViewportAndScissor(
        in_command_buffer,
        swap_chain_info_.extent
        );

    VkDescriptorSet descriptor = WVkRenderUtils::CreateInputRenderDescriptor(
        device_info_.vk_device,
        dspool,
        dslay,
        input_view,
        swap_chain_resources_.InputRenderSampler()
        );

    auto & render_plane = swap_chain_resources_.RenderPlane();
    VkDeviceSize offsets=0;

    vkCmdBindVertexBuffers(in_command_buffer,
                           0,
                           1,
                           &render_plane.vertex_buffer,
                           &offsets);

    vkCmdBindIndexBuffer(in_command_buffer,
                         render_plane.index_buffer,
                         0,
                         VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(in_command_buffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            swap_chain_resources_.PipelineLayout(),
                            0,
                            1,
                            &descriptor,
                            0,
                            nullptr);

    vkCmdDrawIndexed(in_command_buffer,
                     render_plane.index_count,
                     1,
                     0,0,0);

    vkCmdEndRendering(in_command_buffer);

    // Prepare swapchain images for present
    WVkRenderUtils::RndCmd_TransitionRenderImageLayout(
        in_command_buffer,
        swapchain_image,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        {},
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
        );
}
