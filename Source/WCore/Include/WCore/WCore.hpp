#pragma once

#include "LinkDef.hpp"
#include "WCore/WCoreMacros.hpp"
#include "WCoreTypes/WCoreTypes.hpp"
#include "WId.hpp"

#include <cstring>
#include <functional>
#include <cstdint>
#include <cassert>

namespace WIdUtils {

    inline void FromLevelSystemId(const WLevelSystemId & in_src,
                                  WAssetId & out_dst1,
                                  WSystemId & out_dst2) {

        std::uint32_t v = in_src.GetId();
        std::uint16_t lvlid = 0;
        std::uint16_t sysid = 0;

        sysid |= v;
        v >>= sizeof(WSystemId::IdType) * 8;
        lvlid |= v;

        out_dst1 = lvlid;
        out_dst2 = sysid;
    }

    inline WLevelSystemId ToLevelSystemId(const WAssetId & in_level_id,
                                          const WSystemId & in_system_id) {
        std::uint32_t v=0;
        v |= in_level_id.GetId();
        v <<= sizeof(WSystemId::IdType) * 8;
        v |= in_system_id.GetId();

        return v;
    }

}
