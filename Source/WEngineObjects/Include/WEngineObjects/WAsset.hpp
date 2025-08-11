#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WObject.hpp"
#include "WInterfaces/ISerializable.hpp"
#include "WEngineObjects/WClass.hpp"

#include <string>

#include "WAsset.WEngine.hpp"

WCLASS()
class WCORE_API WAsset : public WObject, public ISerializable
{
    
    WOBJECT_BODY(WAsset)

public:

    void Serialize(const std::string & in_path) override;

    void Deserialize(const std::string & in_path) override;

    constexpr WAssetId WID() const noexcept {
        return wid_;
    }

    constexpr void WID(const WAssetId & in_id) noexcept {
        assert(wid_.GetId() == 0);
        wid_ = in_id;
    }

    const char * Name() const noexcept {
        return name_;
    }

    void Name(const char * in_name) noexcept {
        std::strcpy(name_, in_name);
    }

private:

    char name_[WOBJECT_NAME_SIZE]{};

    WAssetId wid_{};

};


