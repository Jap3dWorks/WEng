#include "WImporters.h"
#include "WAssets/WStaticModel.h"
#include "WObjectManager.h"


void WImportObj::Import(const char* file_path, const char* dst_path)
{

};

std::vector<std::string> WImportObj::Extensions()
{
    return {".obj"};
}

std::vector<std::string> WImportObj::Formats()
{
    return {"obj"};
}

