#include "WVulkan/Vk/WVkDescriptor.hpp"

void wvk::descriptor::Create(
    WVkDescriptorSetLayoutInfo& out_descriptor_set_layout_info,
    const VkDevice & in_device
    ) {
    VkDescriptorSetLayoutCreateInfo create_info{};

    create_info.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    create_info.bindingCount = static_cast<uint32_t>(
        out_descriptor_set_layout_info.bindings.size()
        );

    create_info.pBindings =
        out_descriptor_set_layout_info.bindings.data();

    if (vkCreateDescriptorSetLayout(
            in_device,
            &create_info,
            nullptr,
            &out_descriptor_set_layout_info.descset_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }
}

VkDescriptorSetLayout wvk::descriptor::Create(
    VkDescriptorSetLayoutBinding * in_bindings_ptr,
    std::uint32_t in_binding_count,
    VkDevice & in_device
    ) {
    VkDescriptorSetLayout result;
    VkDescriptorSetLayoutCreateInfo create_info{};

    create_info.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    create_info.bindingCount = in_binding_count;
    create_info.pBindings = in_bindings_ptr;

    if (vkCreateDescriptorSetLayout(
            in_device,
            &create_info,
            nullptr,
            &result) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }

    return result;
}


void wvk::descriptor::Create(
    VkDescriptorPool & out_descriptor_pool,
    const VkDevice & in_device)
{
    std::array<VkDescriptorPoolSize,2> pool_sizes;

    pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[0].descriptorCount =
        static_cast<uint32_t>(WVK_MAX_FRAMES_IN_FLIGHT) * 20;  // 40 descriptors in this pool
    pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_sizes[1].descriptorCount =
        static_cast<uint32_t>(WVK_MAX_FRAMES_IN_FLIGHT) * 20;  // 40 descriptors in this pool

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = static_cast<uint32_t>(
        pool_sizes.size()
        );
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = static_cast<uint32_t>(WVK_MAX_FRAMES_IN_FLIGHT * 35); // max sets 70

    if (vkCreateDescriptorPool(
            in_device,
            &pool_info,
            nullptr,
            &out_descriptor_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor pool!");
    }
}

// DEPRECATED
void wvk::descriptor::Create(
    VkDescriptorSet & out_descriptor_set,
    const VkDevice & in_device,
    const WVkDescriptorSetLayoutInfo & descriptor_set_layout_info,
    const VkDescriptorPool & descriptor_pool
    )
{
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &descriptor_set_layout_info.descset_layout;

    if (vkAllocateDescriptorSets(
            in_device,
            &alloc_info,
            &out_descriptor_set
            ) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }
}

VkDescriptorSet wvk::descriptor::CreateDescriptor(
    VkDevice in_device,
    VkDescriptorSetLayout in_descriptor_set_layout,
    VkDescriptorPool in_descriptor_pool
    ) {
    VkDescriptorSet result;
    
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = in_descriptor_pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &in_descriptor_set_layout;

    if (vkAllocateDescriptorSets(
            in_device,
            &alloc_info,
            &result
            ) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }

    return result;
}


void wvk::descriptor::Destroy(
    WVkDescriptorSetLayoutInfo & descriptor_set_layout_info,
    const VkDevice & in_device
    )
{
    // destroy descriptor set layout
    vkDestroyDescriptorSetLayout(
        in_device,
        descriptor_set_layout_info.descset_layout,
        nullptr
        );
}

void wvk::descriptor::Destroy(
    VkDescriptorSetLayout in_descriptor_set_layout,
    VkDevice in_device
    ) {
    vkDestroyDescriptorSetLayout(
        in_device,
        in_descriptor_set_layout,
        nullptr
        );
}

void wvk::descriptor::Destroy(
    VkDescriptorPool & out_descriptor_pool,
    const VkDevice & in_device
    )
{
    vkDestroyDescriptorPool(
        in_device,
        out_descriptor_pool,
        nullptr
        );

    out_descriptor_pool = VK_NULL_HANDLE;
}
