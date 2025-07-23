#include "WImporterRegister.hpp"

void WImporterRegister::ForEach(const TFunction<bool(WImporter*)> & in_fn) const {
    for(auto& p : reg_) {
        if (!in_fn(p.second.get())) {
            break;
        }
    }
}


