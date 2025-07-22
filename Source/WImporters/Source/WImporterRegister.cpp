#include "WImporterRegister.hpp"
#include "WCore/TOptionalRef.hpp"
#include "WEngineInterfaces/IImporter.hpp"

#include <string>

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




