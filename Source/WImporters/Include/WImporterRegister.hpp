#pragma once

#include "WCore/WCore.hpp"
#include "IImporter.hpp"
#include "IImporterRegister.hpp"
#include "WObjectManager/WAssetManagerFacade.hpp"
#include "WObjectManager/WObjectManager.hpp"

#include <utility>
#include <vector>
#include <memory>
#include <concepts>

class WIMPORTERS_API WImporterRegister : public IImporterRegister {
    
public:

    constexpr WImporterRegister(WAssetManagerFacade & in_object_manager) noexcept :
        asset_manager_(in_object_manager),
        registry_() {}
    
    virtual ~WImporterRegister()=default;
    
    WImporterRegister(const WImporterRegister &)=delete;

    constexpr WImporterRegister(WImporterRegister && other) noexcept :
        asset_manager_(std::move(other.asset_manager_)),
        registry_(std::move(other.registry_))
    {}

    WImporterRegister & operator=(const WImporterRegister &) = delete;

    constexpr WImporterRegister & operator=(WImporterRegister && other) noexcept {
        if (this != &other) {
            asset_manager_ = std::move(other.asset_manager_);
            registry_ = std::move(other.registry_);
        }

        return *this;
    }

    void Register(std::unique_ptr<IImporter> && in_importer) override;

    void ForEach(const TFunction<bool(IImporter*)> & in_fn) const override;

    WNODISCARD WAssetManagerFacade & AssetManager() {
        return asset_manager_.Get();
    }

private:

    std::vector<std::unique_ptr<IImporter>> registry_;

    TRef<WAssetManagerFacade> asset_manager_;
    
};
