// #include "IImporterRegister.hpp"
// #include "IImporter.hpp"

// TOptionalRef<IImporter> IImporterRegister::GetImporter(const char * in_extension) const {
//     IImporter* r=nullptr;

//     ForEach([&r, &in_extension](IImporter* _imp) ->bool {
//         for (const std::string & s : _imp->Extensions()) {
//             if (in_extension == s) {
//                 r=_imp;
//                 return false;
//             }
//         }
//         return true;
//     });

//     if (r) {
//         return r;
//     } else {
//         return null_optional_ref;
//     }
// }



