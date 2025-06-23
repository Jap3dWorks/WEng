#include "WImportersRegister.h"
#include "WCore/TOptionalRef.h"

#include "WImporters.h"

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
