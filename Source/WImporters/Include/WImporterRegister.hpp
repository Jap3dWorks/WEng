#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TOptionalRef.hpp"
#include "IImporterRegister.hpp"

#include <utility>
#include <vector>
#include <memory>
#include <concepts>

class IImporter;

class WIMPORTERS_API WImporterRegister : public IImporterRegister {
    
public:

    WImporterRegister() noexcept;
    ~WImporterRegister() override;

    WImporterRegister(const WImporterRegister &)=delete;
    WImporterRegister(WImporterRegister && other) noexcept;

    WImporterRegister & operator=(const WImporterRegister &) = delete;
    WImporterRegister & operator=(WImporterRegister && other) noexcept;

    void Register(std::unique_ptr<IImporter> && in_importer) override;

    void ForEach(const TFunction<bool(IImporter*)> & in_fn) const override;

private:

    std::vector<std::unique_ptr<IImporter>> registry_{};
    
};
