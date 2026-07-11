#include "WObjectDb/WAssetDb.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WString/WString.hpp"

#include <cassert>


WAsset * WAssetDb::Get(std::string_view in_asset_name) const {
    auto split_path = wstr::SplitAssetPath(in_asset_name);
    auto opt_wid = path_tree_.Lookup(split_path);

    if (opt_wid.has_value()) {
        return Get(opt_wid.value().get());
    }
    
    return nullptr;
}
