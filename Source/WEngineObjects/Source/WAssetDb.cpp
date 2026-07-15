#include "WObjectDb/WAssetDb.hpp"
#include "WCore/WId.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WString/WString.hpp"

#include <cassert>

WAsset * WAssetDb::Get(std::string_view asset_name) const {
    auto split_path = wstr::SplitAssetPath(asset_name);
    auto opt_wid = path_tree_.Lookup(split_path);

    wid::WAssetId aid = GetId(asset_name);

    if (aid) {
        return Get(aid);
    }
    else return nullptr;
}

wid::WAssetId WAssetDb::GetId(std::string_view asset_path) const {
    auto split_path = wstr::SplitAssetPath(asset_path);
    auto opt_wid = path_tree_.Lookup(split_path);

    return opt_wid
        .and_then(
            [](auto & itm) -> std::optional<wid::WAssetId>{
                return itm.get();
            })
        .value_or(wid::null_id);
}



