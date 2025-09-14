#pragma once

// #include "WVulkan/WVkRenderConfig.hpp"
// #include "WVulkan/WVulkanStructs.hpp"

#include "WCore/WCore.hpp"
#include "WVulkanStructs.hpp"
#include "WCore/TObjectDataBase.hpp"

class WVkPostprocessPipelines
{
public:

    using WVkRenderPipelineDb = TObjectDataBase<WVkRenderPipelineInfo, void, WAssIdxId>;

public:

    WVkPostprocessPipelines() noexcept =default;

    virtual ~WVkPostprocessPipelines()=default;

    WVkPostprocessPipelines(const WVkPostprocessPipelines &)=delete;

    WVkPostprocessPipelines(WVkPostprocessPipelines &&) noexcept = default;

    WVkPostprocessPipelines & operator=(WVkPostprocessPipelines &&) noexcept = default;
    
    

private:


    WVkRenderPipelineDb pipelines_;

};


