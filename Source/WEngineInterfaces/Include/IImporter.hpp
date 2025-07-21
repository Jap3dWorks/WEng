#pragma once

#include "WCore/WCore.hpp"

#include <memory>
#include <vector>
#include <string>


class IImporter {

public:

    virtual ~IImporter()=default;

    /**
     * Imported assets id. Later you can get the asset using its WId.
     */
    virtual std::vector<WId> Import(const char * file_path, const char * asset_dir)=0;

    virtual std::unique_ptr<IImporter> Clone() const =0;

    constexpr virtual std::vector<std::string> Extensions() const noexcept =0;

    constexpr virtual std::vector<std::string> Formats() const noexcept =0;
  
};
