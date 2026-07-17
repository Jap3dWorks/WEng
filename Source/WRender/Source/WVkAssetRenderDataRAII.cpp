#include "WVulkan/WVkRAII/WVkAssetRenderDataRAII.hpp"
#include "WCore/TVisitor.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/WVk/WVulkan.hpp"
#include "WVulkan/WVk/WVkBuffer.hpp"

#include <algorithm>

WVkAssetRenderDataRAII::~WVkAssetRenderDataRAII() {
    Destroy();
}

WVkAssetRenderDataRAII::WVkAssetRenderDataRAII(
    const VkDevice & device,
    const VkPhysicalDevice & physical_device,
    const VkQueue & graphics_queue,
    const VkCommandPool & command_pool
    )  :
    vkn_(device, physical_device, graphics_queue, command_pool),
    texture_collection_(),
    static_mesh_collection_(),
    ubo_data_()
{}

WVkAssetRenderDataRAII::WVkAssetRenderDataRAII(WVkAssetRenderDataRAII && other) :
    vkn_(std::move(other.vkn_)),
    texture_collection_(std::move(other.texture_collection_)),
    static_mesh_collection_(std::move(other.static_mesh_collection_)),
    ubo_data_(std::move(other.ubo_data_))
{
    other.vkn_ = {};
}

WVkAssetRenderDataRAII & WVkAssetRenderDataRAII::operator=(WVkAssetRenderDataRAII && other) {
    if (this != &other) {
        Destroy();
        
        vkn_ = std::move(other.vkn_);
        texture_collection_ = std::move(other.texture_collection_);
        static_mesh_collection_ = std::move(other.static_mesh_collection_);
        ubo_data_ = std::move(other.ubo_data_);
        
        other.vkn_ = {};
    }

    return *this;
}

void WVkAssetRenderDataRAII::UnloadTexture(const wcr::wid::WAssetId & in_id) {
    texture_collection_.Remove(
        in_id.GetId(),
        [this](WVkTextureInfo & in_texture_info) -> void {
            wvk::texture::DestroyTexture(
                in_texture_info,
                vkn_.device
                );
        }
        );
}

void WVkAssetRenderDataRAII::UnloadStaticMesh(const wcr::wid::WTypeAssetIndexId & in_id) {

    static_mesh_collection_.Remove(
        in_id,
        [this] (WVkMeshInfo & in_mesh_info) -> void {
            wvk::mesh::Destroy(
                in_mesh_info,
                vkn_.device
                );
        });    
}

const WVkMeshInfo & WVkAssetRenderDataRAII::StaticMeshInfo(const wcr::wid::WTypeAssetIndexId & in_id) const {
    return static_mesh_collection_.Get(in_id);
}

void WVkAssetRenderDataRAII::Clear() {
    if (vkn_.device != VK_NULL_HANDLE) {
        texture_collection_.Clear(
            [this](WVkTextureInfo & in_texture_info) -> void {
                wvk::texture::DestroyTexture(
                    in_texture_info,
                    vkn_.device
                    );
            }
            );

        static_mesh_collection_.Clear(
            [this] (WVkMeshInfo & in_mesh_info) -> void {
                wvk::mesh::Destroy(
                    in_mesh_info,
                    vkn_.device
                    );
            });

        ubo_data_.Clear(vkn_.device);
    }
}

void WVkAssetRenderDataRAII::Destroy() {
    if (vkn_.device != VK_NULL_HANDLE) {
        Clear();

        vkn_.device = VK_NULL_HANDLE;
        vkn_.physical_device = VK_NULL_HANDLE;
        vkn_.graphics_queue = VK_NULL_HANDLE;
        vkn_.command_pool = VK_NULL_HANDLE;
    }
}

std::size_t WVkAssetRenderDataRAII::UboData::CreateUBO(
    VkDevice device, VkPhysicalDevice pdevice,
    wcr::wid::WEngId id,
    std::size_t ubo_size, void* initial_data) {

    std::size_t ubo_id = ubo_collection.Create(
        [device, pdevice, &ubo_size, &initial_data, this]
        (std::size_t id) {
            WVkUBOInfo ubo = wvk::buffer::CreateUBO(
                ubo_size,
                device,
                pdevice
                );

            if (initial_data) {
                wvk::buffer::UpdateUBO(
                    wvk::buffer::MapUBO(ubo, device),
                    initial_data,
                    ubo_size,
                    0
                    );

                wvk::buffer::UnmapUBO(ubo, device);
            }
            return ubo;
        }
        );

    Reg(id, ubo_id);
    
    return ubo_id;
}

void WVkAssetRenderDataRAII::UboData::Clear(VkDevice device) {
    ubo_collection.Clear(
        [this, &device](WVkUBOInfo & ubo_info) -> void {
            wvk::buffer::Destroy(
                ubo_info, device
                );
        }
        );

    wengid_ubos.clear();
}

void WVkAssetRenderDataRAII::UboData::Reg(wcr::wid::WEngId wid, std::size_t ubo_id) {
    if (!wengid_ubos.contains(wid)) {
        wengid_ubos[wid]=ubo_id;
    } else {
        std::visit(
            wcr::TVisitor(
                [this, wid, ubo_id](std::size_t old_id) {
                    wengid_ubos[wid]=std::vector{old_id, ubo_id};
                },
                [this, ubo_id](std::vector<std::size_t> & ids) {
                    ids.push_back(ubo_id);
                }
                ),
            wengid_ubos[wid]
            );
    }
}

void WVkAssetRenderDataRAII::UboData::DestroyUBOs(wcr::wid::WEngId wid, VkDevice device) {
    auto & ubo_ids = wengid_ubos[wid];

    std::visit(
        wcr::TVisitor(
            [this, device](std::size_t ubo_id) {
                ubo_collection.Remove(
                    ubo_id,
                    [this, device] (WVkUBOInfo & ubo_info) {
                        wvk::buffer::Destroy(ubo_info, device);
                    }
                    );
            },
            [this, device](std::vector<std::size_t> & ubo_ids) {
                std::for_each(
                    ubo_ids.begin(),
                    ubo_ids.end(),
                    [this, device](std::size_t ubo_id) {
                        ubo_collection.Remove(
                            ubo_id,
                            [this, device](WVkUBOInfo & ubo_info) {
                                wvk::buffer::Destroy(ubo_info, device);
                            });
                    });
            }
            ),
        ubo_ids
        );

    wengid_ubos.erase(wid);
}

