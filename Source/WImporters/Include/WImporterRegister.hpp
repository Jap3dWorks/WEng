#pragma once

#include "WCore/WCore.hpp"
#include "WEngineInterfaces/IImporter.hpp"
#include "WEngineInterfaces/IImporterRegister.hpp"
#include "WObjectManager/WAssetManagerFacade.hpp"
#include "WObjectManager/WObjectManager.hpp"

#include <utility>
#include <vector>
#include <memory>
#include <concepts>

class WIMPORTERS_API WImporterRegister : public IImporterRegister {
    
public:

    constexpr WImporterRegister() noexcept :
        registry_() {}
    
    virtual ~WImporterRegister()=default;
    
    WImporterRegister(const WImporterRegister &)=delete;

    constexpr WImporterRegister(WImporterRegister && other) noexcept :
        registry_(std::move(other.registry_))
    {}

    WImporterRegister & operator=(const WImporterRegister &) = delete;

    constexpr WImporterRegister & operator=(WImporterRegister && other) noexcept {
        if (this != &other) {
            registry_ = std::move(other.registry_);
        }

        return *this;
    }

    void Register(std::unique_ptr<IImporter> && in_importer) override;

    void ForEach(const TFunction<bool(IImporter*)> & in_fn) const override;

private:

    std::vector<std::unique_ptr<IImporter>> registry_;
    
};
