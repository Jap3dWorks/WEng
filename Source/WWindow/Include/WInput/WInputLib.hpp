#pragma once

#include "WCore/WCore.hpp"
#include "WCoreTypes/WEngineStructs.hpp"

namespace WInputLib {

    EInputKey ToInputKey(std::int32_t key);

    EInputMode ToInputMode(const std::int32_t & value);

    inline WInput ToWInputMode(const std::int32_t & in_key, const std::int32_t & in_mode) {
        return {ToInputKey(in_key), ToInputMode(in_mode)};
    }

}
