#include "WVulkan/WVk/WVkTexture.hpp"
#include "WStructs/WTextureStructs.hpp"
#include "WVulkan/WVk/WVkBuffer.hpp"
#include "WVulkan/WVk/WVkImage.hpp"
#include "WVulkan/WVk/WVulkan.hpp"

void wvk::texture::CreateTexture(
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
        texture_rgba = wvk::texture::AddRGBAPadding(texture_struct);
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
    wvk::buffer::CreateVkBuffer(
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

    wvk::image::CreateImage(
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

    wvk::image::TransitionImageLayout(
        in_device,
        in_command_pool,
        in_graphics_queue,
        out_texture_info.image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        out_texture_info.mip_levels
        );

    // This operation is using DMA.
    wvk::image::CopyBufferToImage(
        staging_buffer,
        out_texture_info.image,
        texture_ptr->width,
        texture_ptr->height,
        in_device,
        in_command_pool,
        in_graphics_queue
        );

    wvk::image::GenerateMipmaps(
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
    out_texture_info.view = wvk::image::CreateImageView(
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


VkSampler wvk::texture::CreateTextureSampler(
    const VkDevice& device, 
    const VkPhysicalDevice& physical_device,
    const uint32_t& mip_levels
    ) {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physical_device, &properties);

    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.anisotropyEnable = VK_TRUE;
    sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.minLod = 0.f;
    sampler_info.maxLod = static_cast<float>(mip_levels);
    sampler_info.mipLodBias = 0.f;

    VkSampler texture_sampler;
    if (vkCreateSampler(device, &sampler_info, nullptr, &texture_sampler) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create texture sampler!");
    }

    return texture_sampler;
}

void wvk::texture::DestroyTexture(
    WVkTextureInfo & in_texture_info,
    const VkDevice & in_device
    ) {
    
    vkDestroySampler(in_device,
                     in_texture_info.sampler,
                     nullptr);

    in_texture_info.sampler = VK_NULL_HANDLE;

    vkDestroyImageView(in_device,
                       in_texture_info.view,
                       nullptr);

    in_texture_info.view = VK_NULL_HANDLE;

    vkDestroyImage(in_device,
                   in_texture_info.image,
                   nullptr);

    in_texture_info.image = VK_NULL_HANDLE;

    vkFreeMemory(in_device,
                 in_texture_info.memory,
                 nullptr);

    in_texture_info.memory = VK_NULL_HANDLE;
}


void wvk::texture::DestroyVkSampler(
    VkSampler & out_sampler,
    const VkDevice & in_device
    ) {
    vkDestroySampler(in_device, out_sampler, nullptr);
    out_sampler = VK_NULL_HANDLE;
}


VkFormat wvk::texture::ToVkFormat(ETextureFormat in_texture_format)
{
    // TODO fill with remaining formats

    switch(in_texture_format) {
    case ETextureFormat::R8_SRGB:
        return VK_FORMAT_R8_SRGB;
    case ETextureFormat::RG8_SRGB:
        return VK_FORMAT_R8G8_SRGB;
    case ETextureFormat::RGB8_SRGB:
        return VK_FORMAT_R8G8B8_SRGB;
    case ETextureFormat::RGBA8_SRGB:
        return VK_FORMAT_R8G8B8A8_SRGB;
    default:
        return VK_FORMAT_R8G8B8A8_SRGB;
    }
}

WTextureStruct wvk::texture::AddRGBAPadding(const WTextureStruct & in_texture)
{
    WTextureStruct result;
    
    result.format = ETextureFormat::RGBA8_SRGB;
    result.height = in_texture.height;
    result.width = in_texture.width;

    result.data = in_texture.data;

    size_t tex_size = result.height * result.width;

    result.data.resize(result.height * result.width * 4, 255);

    int num_channels = NumOfChannels(in_texture.format);

    for (int i=num_channels; i < 4; i++) {
        std::memcpy(
            result.data.data() + (tex_size * i),
            result.data.data(),
            tex_size
            );
    }

    return result;
}

