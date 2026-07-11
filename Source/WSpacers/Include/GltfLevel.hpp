#pragma once

#include "WEngine/WEngine.hpp"
#include "WEngine/WEngineDefaults.hpp"

#include "WAssets/Level.hpp"
#include "WImporter/WImporterGltf.hpp"
#include "WObjectDb/WAssetDb.hpp"
#include <algorithm>

namespace spacers::gltflevel {

    inline void SetupLighting(was::Level * out_level) {}

    inline WAssetId CreateLevel(WEngine & in_engine) {
        std::vector<WAssetId> imported_assets = in_engine
            .ImportersRegister()
            .GetImporter<wim::importer::WImporterGltf>()
            .Import(
                in_engine.AssetManager(),
                "Content/Assets/Levels/ABeautifulGame.glb",
                "/Content/Assets/ImportedLevels/"
                );

        WAssetId level_id{wid_null};
        was::Level * level_asset{nullptr};

        for (auto & item : imported_assets) {
            if (in_engine.AssetManager().Get(item)->Class()->IsEqual(was::Level::StaticClass())) {
                level_id = item;
                level_asset = in_engine
                    .AssetManager()
                    .Get<was::Level>(item);

                break;
            }
        }

        SetupLighting(level_asset);

        return level_id;

    }
}
