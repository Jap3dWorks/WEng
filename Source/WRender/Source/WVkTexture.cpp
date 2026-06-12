#include "WVulkan/WVkTexture.hpp"
#include "WVulkan/WVkUtils/WVulkan.hpp"

void weng::vk::texture::CreateTexture(
    WVkTextureInfo &out_texture_info,
    const WTextureStruct &texture_struct,
    const VkDevice & in_device,
    const VkPhysicalDevice & in_physical_device,
    const VkQueue & in_graphics_queue,
    const VkCommandPool &in_command_pool
    )
{
    
    WTextureStruct texture_rgba;
    const WTextureStruct * texture_ptr;

    // Textures must be RGBA, graphic cards prefer RGBA padding.
    auto num_channels = NumOfChannels(texture_struct.format);
    if (4==num_channels) {
        texture_ptr = &texture_struct;
    }
    else
    {
        texture_rgba = weng::vk::vulkan::AddRGBAPadding(texture_struct);
        texture_ptr = &texture_rgba;
    }

    assert(
        texture_ptr->data.size() ==
        (texture_ptr->width * texture_ptr->height * NumOfChannels(texture_ptr->channels))
        );

    out_texture_info.mip_levels =
        static_cast<uint32_t>(
            std::floor(
                std::log2(
                    std::max(texture_ptr->width, texture_ptr->height)))) + 1;

    VkDeviceSize image_size = texture_ptr->height * texture_ptr->width * 4;

    // staging buffers are host accesible ram
    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    CreateVkBuffer(
        staging_buffer,
        staging_buffer_memory,
        in_device,
        in_physical_device,
        image_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

    void * data;
    vkMapMemory(in_device, staging_buffer_memory, 0, image_size, 0, &data);
    memcpy(
        data,
        texture_ptr->data.data(),
        std::min(texture_ptr->data.size(), static_cast<size_t>(image_size))
        );
    vkUnmapMemory(in_device, staging_buffer_memory);

    CreateImage(
        out_texture_info.image,
        out_texture_info.memory,
        in_device,
        in_physical_device,
        texture_ptr->width,
        texture_ptr->height,
        out_texture_info.mip_levels,
        VK_SAMPLE_COUNT_1_BIT,
        VK_FORMAT_R8G8B8A8_SRGB,  // TODO image format as a param
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

    TransitionTextureImageLayout(
        in_device,
        in_command_pool,
        in_graphics_queue,
        out_texture_info.image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        out_texture_info.mip_levels
        );

    // This operation is using DMA.
    CopyBufferToImage(
        staging_buffer,
        out_texture_info.image,
        texture_ptr->width,
        texture_ptr->height,
        in_device,
        in_command_pool,
        in_graphics_queue
        );

    GenerateMipmaps(
        out_texture_info.image,
        VK_FORMAT_R8G8B8A8_SRGB,
        texture_ptr->width,
        texture_ptr->height,
        out_texture_info.mip_levels,
        in_device,
        in_physical_device,
        in_command_pool,
        in_graphics_queue
        );

    // Image view
    out_texture_info.view = CreateImageView(
        out_texture_info.image,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_ASPECT_COLOR_BIT,
        out_texture_info.mip_levels,
        in_device
        );

    // Sampler
    out_texture_info.sampler = CreateTextureSampler(
        in_device,
        in_physical_device,
        out_texture_info.mip_levels
        );

    vkFreeMemory(in_device, staging_buffer_memory, nullptr);
    vkDestroyBuffer(in_device, staging_buffer, nullptr);
}



