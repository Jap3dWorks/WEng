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

// Manage the aquirement and release of texture or mesh buffer and stuffs like that.

template<typename D,  /* typename A, */ CIsWId WIdType=WAssetId>
class WRENDER_API WVkResourceCollection
{
public:

    // using CreateFn = TFunction<D(const WIdType & id)>;
    using ClearFn = TFunction<void(D&)>;
    using WVkAssetDb = TObjectDataBase<D, void, WIdType>;

public:

    constexpr WVkResourceCollection() noexcept :
    resources_(),
    vulkan_data_()
    {}

    constexpr WVkResourceCollection(ClearFn in_clear_fn) :
    vulkan_data_(in_clear_fn) {}

    virtual ~WVkResourceCollection() { Clear(); }

    WVkResourceCollection(const WVkResourceCollection & other) :
    resources_(other.resources_),
    vulkan_data_(other.vulkan_data_)
    {}

    constexpr WVkResourceCollection(WVkResourceCollection && other) noexcept :
    resources_(std::move(other.resources_)),
    vulkan_data_(std::move(other.vulkan_data_))
    {}

    WVkResourceCollection & operator=(const WVkResourceCollection & other) {
        if (this != &other) {
            resources_ = other.resources_;
            vulkan_data_ = other.vulkan_data_;            
        }

        return *this;
    }

    constexpr WVkResourceCollection & operator=(WVkResourceCollection && other) noexcept {
        if (this != &other) {
            resources_ = std::move(other.resources_);
            vulkan_data_ = std::move(other.vulkan_data_);
        }

        return *this;
    }

    // void SetCreateFn(const CreateFn & in_fn) {
    //     vulkan_data_.SetCreateFn(in_fn);
    // }

    void SetClearFn(const ClearFn & in_fn) {
        vulkan_data_.SetDestroyFn(in_fn);
    }

    // /** Assign a WIdType to identify the asset */
    // void RegisterResource(A & in_asset, const WIdType & in_id) {
    //     resources_.insert(in_id);
    //     // resources_.insert({in_id, in_asset});
    // }

    // void UnregisterResource(const WIdType & in_id) {
    //     assert(resources_.contains(in_id));
    //     resources_.erase(in_id);
    // }

    // void UnregisterResource(const A & in_asset) {
    //     UnregisterResource(in_asset.Wid());
    // }

    template<CCallable<D, const WIdType &> Fn>
    void LoadResource(const WIdType & in_id, const Fn & create_fn) {
        assert(resources_.contains(in_id));
        vulkan_data_.CreateAt(in_id, create_fn);
        // vulkan_data_.CreateAt(in_id);
    }

    // void LoadResource(const A & in_asset) {
    //     LoadResource(in_asset.Wid());
    // }

    /** pop from graphical memory */
    void UnloadResource(const WIdType & in_id) {
        assert(resources_.contains(in_id));
        vulkan_data_.Remove(in_id);
    }

    // void UnloadResource(const WIdType & in_asset) {
    //     UnloadResource(in_asset);
    // }

    const D & GetData(const WIdType & in_id) const noexcept {
        assert(data_.Contains(in_id));
        return vulkan_data_.Get(in_id);
    }

    bool Contains(const WIdType & in_id) const noexcept {
        return vulkan_data_.Contains(in_id);
    }

    // const A & GetResource(const WIdType & in_id) const {
    //     assert(assets_.contains(in_id));
    //     return assets_.at(in_id).Get();
    // }

    void Clear() {
        resources_.clear();
        vulkan_data_.Clear();
    }

protected:

private:

    // TWRef to ensure that the reference to asset is alive
    // std::unordered_map<WIdType, TWRef<A>> assets_;
    // std::unordered_map<WIdType, A*> resources_;

    std::unordered_set<WIdType> resources_{};

    WVkAssetDb vulkan_data_;

};

