#pragma once

#include "WCore.h"
#include <vector>

class WIMPORTERS_API WImporter
{
public:
    WImporter() = default;
    virtual ~WImporter() = default;

    virtual void Import(const char* file_path) = 0;

protected:
    virtual std::vector<std::string> Extensions() = 0;
    virtual std::vector<std::string> Formats() = 0;
    
    // virtual std::vector<WObject*> Objects() = 0;
};


class WIMPORTERS_API WImportObj : public WImporter
{
public:
    WImportObj() = default;
    virtual ~WImportObj() = default;

    void Import(const char* file_path);

protected:
    std::vector<std::string> Extensions();
    std::vector<std::string> Formats();
    
    // std::vector<WObject*> Objects{};
};
