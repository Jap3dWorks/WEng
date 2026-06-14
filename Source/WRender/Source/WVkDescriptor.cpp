#include "WVulkan/WVk/WVkDescriptor.hpp"


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
