#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WCore/TFunction.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WCore/WConcepts.hpp"

#include <unordered_map>
#include <cassert>
#include <concepts>

/**
 * @brief Manage the aquirement and release of texture or mesh buffer and stuffs like that.
 */
template<typename D, CIsWId WIdType=WAssetId>
class WRENDER_API WVkResourceCollection
{
public:

    using ClearFn = TFunction<void(D&)>;
    using WVkAssetDb = TObjectDataBase<D, void, WIdType>;

public:

    constexpr WVkResourceCollection() noexcept :
    vulkan_data_()
    {}

    constexpr WVkResourceCollection(ClearFn in_clear_fn) :
    vulkan_data_(in_clear_fn) {}

    virtual ~WVkResourceCollection() { Clear(); }

    WVkResourceCollection(const WVkResourceCollection & other) :
    vulkan_data_(other.vulkan_data_)
    {}

    constexpr WVkResourceCollection(WVkResourceCollection && other) noexcept :
    vulkan_data_(std::move(other.vulkan_data_))
    {}

    WVkResourceCollection & operator=(const WVkResourceCollection & other) {
        if (this != &other) {
            vulkan_data_ = other.vulkan_data_;            
        }

        return *this;
    }

    constexpr WVkResourceCollection & operator=(WVkResourceCollection && other) noexcept {
        if (this != &other) {
            vulkan_data_ = std::move(other.vulkan_data_);
        }

        return *this;
    }

    void SetClearFn(const ClearFn & in_fn) {
        vulkan_data_.SetDestroyFn(in_fn);
    }

    template<CCallable<D, const WIdType &> Fn>
    void LoadResource(const WIdType & in_id, const Fn & create_fn) {
        assert(!Contains(in_id));
        vulkan_data_.CreateAt(in_id, create_fn);
    }

    /** pop from graphical memory */
    void UnloadResource(const WIdType & in_id) {
        assert(Contains(in_id));
        vulkan_data_.Remove(in_id);
    }

    const D & GetData(const WIdType & in_id) const noexcept {
        assert(vulkan_data_.Contains(in_id));
        return vulkan_data_.Get(in_id);
    }

    bool Contains(const WIdType & in_id) const noexcept {
        return vulkan_data_.Contains(in_id);
    }

    void Clear() {
        vulkan_data_.Clear();
    }

protected:

private:

    WVkAssetDb vulkan_data_;

};

