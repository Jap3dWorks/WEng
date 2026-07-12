#include "WObjectDb/WAssetDb.hpp"
#include "WCore/WId.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WString/WString.hpp"

#include <cassert>


WAsset * WAssetDb::Get(std::string_view in_asset_name) const {
    auto split_path = wstr::SplitAssetPath(in_asset_name);
    auto opt_wid = path_tree_.Lookup(split_path);

    WAssetTypeId atype = wclass_track_.GetAssetTypeId(opt_wid.value().get());
    
    WTypeAssetIndexId idvalue{atype, opt_wid.value().get(), WID_NULL_V};

    if (opt_wid.has_value()) {
        return Get(idvalue);
    }
    
    return nullptr;
}


