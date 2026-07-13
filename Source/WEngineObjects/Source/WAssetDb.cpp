#include "WObjectDb/WAssetDb.hpp"
#include "WCore/WId.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WString/WString.hpp"

#include <cassert>

WAsset * WAssetDb::Get(std::string_view asset_name) const {
    auto split_path = wstr::SplitAssetPath(asset_name);
    auto opt_wid = path_tree_.Lookup(split_path);

    if (opt_wid.has_value()) {
        wid::WAssetTypeId atype = wclass_track_
            .GetAssetTypeId(opt_wid.value().get());

        wid::WTypeAssetIndexId idvalue{atype,
                                       opt_wid.value().get(),
                                       wid::NULL_V};

        return Get(idvalue);
    }
    
    return nullptr;
}

bool WAssetDb::ExistsAsset(std::string_view asset_path) const {
    auto split_path = wstr::SplitAssetPath(asset_path);
    return path_tree_.Lookup(split_path).has_value();
}


