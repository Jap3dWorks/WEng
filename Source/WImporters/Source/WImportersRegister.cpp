#include "WImportersRegister.hpp"
#include "WCore/TOptionalRef.hpp"

#include "WImporters.hpp"

#include <string>


TOptionalRef<WImporter> WImportersRegister::GetImporter(const char * in_extension)
{
    for (auto & importer : *this) {
        for (const std::string& s : importer->Extensions()) {
            if (in_extension == s) {
                return importer.get();
            }
        }
    }
    
    return null_optional_ref;
}
