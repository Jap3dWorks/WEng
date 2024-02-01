#include "WRenderPipeline.h"
#include <cassert>
#include <stdexcept>

// WRenderPipelineInfo
// -------------------

WRenderPipelineInfo::~WRenderPipelineInfo()
{
}

// WRenderPipelines
// -------------------

WRenderPipelineInfo& WRenderPipelines::CreateRenderPipeline(WRenderPipelineInfo info)
{
    if (info.pipeline != nullptr) {
        throw std::logic_error("info.pipeline must be nullptr");
    }
    if (info.pipeline_layout != nullptr) {
        throw std::logic_error("info.pipeline_layout must be nullptr");
    }
    if (info.descriptor_set_layout == nullptr) {
        throw std::logic_error("info.descriptor_set_layout must be not nullptr");
    }

    // Create Vulkan Pipeline into info object
    WVulkan::CreateVkRenderPipeline(device_, info);

    render_pipelines_[info.type].push_back(std::move(info));

    return render_pipelines_[info.type].back();
}

WDescriptorSetLayoutInfo& WRenderPipelines::CreateDescriptorSetLayout(WDescriptorSetLayoutInfo info)
{
    if (info.descriptor_set_layout != nullptr) {
        throw std::logic_error("info.descriptor_set_layout must be nullptr");
    }

    // Create Vulkan Descriptor Set Layout into info object
    WVulkan::CreateVkDescriptorSetLayout(device_, info);

    descriptor_set_layouts_.push_back(std::move(info));

    return descriptor_set_layouts_.back();
}

WRenderPipelines::~WRenderPipelines()
{
    for (auto& pipeline_type : render_pipelines_)
    {
        for (auto& pipeline : pipeline_type.second)
        {
            WVulkan::DestroyVkRenderPipeline(device_, pipeline);
        }
    }
    for(auto& descriptor_set_layout : descriptor_set_layouts_)
    {
        WVulkan::DestroyDescriptorSetLayout(device_, descriptor_set_layout);
    }
}

// WVulkan 
// ---------------

void WVulkan::CreateVkRenderPipeline(WDeviceInfo device, WRenderPipelineInfo& out_pipeline_info)
{
    // Create Shader Stages
    std::vector<VkPipelineShaderStageCreateInfo> ShaderStages(
        out_pipeline_info.shaders.size()
    );
    std::vector<VkShaderModule> ShaderModules(
        out_pipeline_info.shaders.size()
    );

    WShaderStage *vertex_shader_stage = nullptr;

    for (uint32_t i = 0; i < out_pipeline_info.shaders.size(); i++)
    {
        ShaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ShaderStages[i].stage = WVulkan::ToShaderStageFlagBits(
            out_pipeline_info.shaders[i].type
        );
        
        if(out_pipeline_info.shaders[i].type == WShaderType::Vertex)
        {
            vertex_shader_stage = &out_pipeline_info.shaders[i];
        }

        ShaderModules[i] = WVulkan::CreateShaderModule(
            device,
            out_pipeline_info.shaders[i]
        );

        ShaderStages[i].module = ShaderModules[i];
        ShaderStages[i].pName = out_pipeline_info.shaders[i].entry_point.c_str();
    }

    if (vertex_shader_stage == nullptr)
    {
        throw std::runtime_error("Vertex shader stage not found!");
    }

    VkPipelineVertexInputStateCreateInfo VertexInputInfo{};
    VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(
        vertex_shader_stage->binding_descriptors.size()
    );
    VertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>( 
        vertex_shader_stage->attribute_descriptors.size()
    );
    VertexInputInfo.pVertexBindingDescriptions = 
        vertex_shader_stage->binding_descriptors.data();
    VertexInputInfo.pVertexAttributeDescriptions = 
        vertex_shader_stage->attribute_descriptors.data();

    VkPipelineInputAssemblyStateCreateInfo InputAssembly{};
    InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    InputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo ViewportState{};
    ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    ViewportState.viewportCount = 1;
    ViewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo Rasterizer{};
    Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    Rasterizer.depthClampEnable = VK_FALSE;
    Rasterizer.rasterizerDiscardEnable = VK_FALSE;
    Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    Rasterizer.lineWidth = 1.0f;
    Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    Rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // because of y-flip in the projection matrix
    Rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo Multisampling{};
    Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    Multisampling.sampleShadingEnable = VK_FALSE;
    Multisampling.rasterizationSamples = out_pipeline_info.sample_count;

    VkPipelineDepthStencilStateCreateInfo DepthStencil{};
    DepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    DepthStencil.depthTestEnable = VK_TRUE;
    DepthStencil.depthWriteEnable = VK_TRUE;
    DepthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    DepthStencil.depthBoundsTestEnable = VK_FALSE;
    DepthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState ColorBlendAttachment{};
    ColorBlendAttachment.colorWriteMask = 
        VK_COLOR_COMPONENT_R_BIT | 
        VK_COLOR_COMPONENT_G_BIT | 
        VK_COLOR_COMPONENT_B_BIT | 
        VK_COLOR_COMPONENT_A_BIT;
    ColorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo ColorBlending{};
    ColorBlending.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    ColorBlending.logicOpEnable = VK_FALSE;
    ColorBlending.logicOp = VK_LOGIC_OP_COPY;
    ColorBlending.attachmentCount = 1;
    ColorBlending.pAttachments = &ColorBlendAttachment;
    ColorBlending.blendConstants[0] = 0.f;
    ColorBlending.blendConstants[1] = 0.f;
    ColorBlending.blendConstants[2] = 0.f;
    ColorBlending.blendConstants[3] = 0.f;

    std::vector<VkDynamicState> DynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo DynamicState{};
    DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    DynamicState.dynamicStateCount = static_cast<uint32_t>(DynamicStates.size());
    DynamicState.pDynamicStates = DynamicStates.data();

    VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.setLayoutCount = 0;
    PipelineLayoutInfo.pSetLayouts = &out_pipeline_info.descriptor_set_layout; // DescriptorSetLayout

    if (vkCreatePipelineLayout(
        device.vk_device,
        &PipelineLayoutInfo,
        nullptr,
        &out_pipeline_info.pipeline_layout
    ) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo PipelineInfo{};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    PipelineInfo.stageCount = static_cast<uint32_t>(ShaderStages.size());
    PipelineInfo.pStages = ShaderStages.data();
    PipelineInfo.pVertexInputState = &VertexInputInfo;
    PipelineInfo.pInputAssemblyState = &InputAssembly;
    PipelineInfo.pViewportState = &ViewportState;
    PipelineInfo.pRasterizationState = &Rasterizer;
    PipelineInfo.pMultisampleState = &Multisampling;
    PipelineInfo.pDepthStencilState = &DepthStencil;
    PipelineInfo.pColorBlendState = &ColorBlending;
    PipelineInfo.pDynamicState = &DynamicState;
    PipelineInfo.layout = out_pipeline_info.pipeline_layout;
    PipelineInfo.renderPass = out_pipeline_info.render_pass;
    PipelineInfo.subpass = 0;
    PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(
        device.vk_device,
        VK_NULL_HANDLE,
        1,
        &PipelineInfo,
        nullptr,
        &out_pipeline_info.pipeline
    ) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create graphics pipeline!");
    }

    // Destroy Shader Modules
    for (auto& shader_module : ShaderModules)
    {
        vkDestroyShaderModule(
            device.vk_device,
            shader_module,
            nullptr
        );
    }
}

void WVulkan::CreateVkDescriptorSetLayout(WDeviceInfo device, WDescriptorSetLayoutInfo& out_descriptor_set_layout_info)
{
    VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutInfo{};
    DescriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    DescriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(
        out_descriptor_set_layout_info.bindings.size()
    );
    DescriptorSetLayoutInfo.pBindings = out_descriptor_set_layout_info.bindings.data();

    if (vkCreateDescriptorSetLayout(
        device.vk_device,
        &DescriptorSetLayoutInfo,
        nullptr,
        &out_descriptor_set_layout_info.descriptor_set_layout
    ) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }
}

void WVulkan::DestroyVkRenderPipeline(WDeviceInfo device, WRenderPipelineInfo& out_pipeline_info)
{
    if (pipeline.pipeline_layout)
    {
        // destroy pipeline layout
        vkDestroyPipelineLayout(
            device_.vk_device,
            pipeline.pipeline_layout,
            nullptr
        );
    }

    if (pipeline.pipeline)
    {   
        // destroy pipeline
        vkDestroyPipeline(
            device_.vk_device,
            pipeline.pipeline_layout,
            nullptr
        );
    }
}

void WVulkan::DestroyDescriptorSetLayout(WDeviceInfo device, const WDescriptorSetLayoutInfo& descriptor_set_layout_info)
{
    // destroy descriptor set layout
    vkDestroyDescriptorSetLayout(
        device_.vk_device,
        descriptor_set_layout.descriptor_set_layout,
        nullptr
    );
}

