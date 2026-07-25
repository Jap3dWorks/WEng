#include "WVulkan/RAII/AssetRenderData.hpp"
#include "WCore/TVisitor.hpp"
#include "WCore/WCore.hpp"
#include "WVulkan/WVulkanStructs.hpp"
#include "WVulkan/Vk/WVulkan.hpp"
#include "WVulkan/Vk/WVkBuffer.hpp"

#include <algorithm>

wvk::raii::AssetRenderData::~AssetRenderData() {
    Destroy();
}

wvk::raii::AssetRenderData::AssetRenderData(
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

wvk::raii::AssetRenderData::AssetRenderData(wvk::raii::AssetRenderData && other) :
    vkn_(std::move(other.vkn_)),
    texture_collection_(std::move(other.texture_collection_)),
    static_mesh_collection_(std::move(other.static_mesh_collection_)),
    ubo_data_(std::move(other.ubo_data_))
{
    other.vkn_ = {};
}

wvk::raii::AssetRenderData & wvk::raii::AssetRenderData::operator=(wvk::raii::AssetRenderData && other) {
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

void wvk::raii::AssetRenderData::UnloadTexture(const wcr::wid::WAssetId & in_id) {
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

void wvk::raii::AssetRenderData::UnloadStaticMesh(const wcr::wid::WTypeAssetIndexId & in_id) {

    static_mesh_collection_.Remove(
        in_id,
        [this] (WVkMesh & in_mesh_info) -> void {
            wvk::mesh::Destroy(
                in_mesh_info,
                vkn_.device
                );
        });    
}

const WVkMesh & wvk::raii::AssetRenderData::StaticMeshInfo(const wcr::wid::WTypeAssetIndexId & in_id) const {
    return static_mesh_collection_.Get(in_id);
}

void wvk::raii::AssetRenderData::Clear() {
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
            [this] (WVkMesh & in_mesh_info) -> void {
                wvk::mesh::Destroy(
                    in_mesh_info,
                    vkn_.device
                    );
            });

        ubo_data_.Clear(vkn_.device);
    }
}

void wvk::raii::AssetRenderData::Destroy() {
    if (vkn_.device != VK_NULL_HANDLE) {
        Clear();
        vkn_.device = VK_NULL_HANDLE;
        vkn_.physical_device = VK_NULL_HANDLE;
        vkn_.graphics_queue = VK_NULL_HANDLE;
        vkn_.command_pool = VK_NULL_HANDLE;
    }
}

std::vector<std::size_t> wvk::raii::AssetRenderData::GetUBOs(wcr::wid::WEngId wid) const {
    std::vector<std::size_t> result;

    std::visit(
        wcr::TVisitor(
            [this, &result] (std::size_t idx) {
                result.push_back(idx);
            },
            [this, &result] (std::vector<size_t> idxes) {
                for (auto i : idxes) {
                    result.push_back(i);
                }
            }
            ),
        ubo_data_.ubo_sets.at(wid)
        );

    return result;
}

std::size_t wvk::raii::AssetRenderData::UboData::CreateUBO(
    VkDevice device,
    VkPhysicalDevice pdevice,
    wcr::wid::WEngId ubo_set_id,
    std::size_t ubo_size,
    void const * initial_data) {

    std::size_t ubo_id = ubo_collection.Create(
        [device, pdevice, &ubo_size, &initial_data, this]
        (std::size_t id) {
            WVkUBO ubo = wvk::buffer::CreateUBO(
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

    Reg(ubo_set_id, ubo_id);
    
    return ubo_id;
}

void wvk::raii::AssetRenderData::UboData::Clear(VkDevice device) {
    ubo_collection.Clear(
        [this, &device](WVkUBO & ubo_info) -> void {
            wvk::buffer::Destroy(
                ubo_info, device
                );
        }
        );

    ubo_sets.clear();
}

void wvk::raii::AssetRenderData::UboData::Reg(wcr::wid::WEngId ubo_set_id, std::size_t ubo_id) {
    if (!ubo_sets.contains(ubo_set_id)) {
        
        ubo_sets[ubo_set_id]=ubo_id;

    } else {
        std::visit(
            wcr::TVisitor(
                [this, ubo_set_id, ubo_id](std::size_t old_id) {
                    ubo_sets[ubo_set_id]=std::vector{old_id, ubo_id};
                },
                [this, ubo_id](std::vector<std::size_t> & vector_ids) {
                    vector_ids.push_back(ubo_id);
                }
                ),
            ubo_sets[ubo_set_id]
            );
    }
}

void wvk::raii::AssetRenderData::UboData::DestroyUBOs(wcr::wid::WEngId wid, VkDevice device) {
    auto & ubo_ids = ubo_sets[wid];

    std::visit(
        wcr::TVisitor(
            [this, device](std::size_t ubo_id) {
                ubo_collection.Remove(
                    ubo_id,
                    [this, device] (WVkUBO & ubo_info) {
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
                            [this, device](WVkUBO & ubo_info) {
                                wvk::buffer::Destroy(ubo_info, device);
                            });
                    });
            }
            ),
        ubo_ids
        );

    ubo_sets.erase(wid);
}

