#include "WImporterRegister.hpp"
#include "WCore/TOptionalRef.hpp"
#include "IImporter.hpp"

#include <string>

WImporterRegister::WImporterRegister() noexcept = default;

WImporterRegister::~WImporterRegister() = default;

WImporterRegister::WImporterRegister(WImporterRegister && other) noexcept=default;

WImporterRegister & WImporterRegister::operator=(WImporterRegister && other) noexcept = default;

void WImporterRegister::Register(std::unique_ptr<IImporter> && in_importer) {
    registry_.push_back(std::move(in_importer));
}

void WImporterRegister::ForEach(const TFunction<bool(IImporter*)> & in_fn) const {
    for(auto& _imp : registry_) {
        if (!in_fn(_imp.get())) {
            break;
        }
    }
}
