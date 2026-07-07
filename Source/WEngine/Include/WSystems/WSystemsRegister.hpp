#pragma once

#include "WCore/WCore.hpp"
#include "WSystems/WSystems.hpp"
#include "WCore/TSparseSet.hpp"
#include "WCore/IdPool.hpp"

#include <unordered_map>
#include <string>

// TODO All WSystems registry, active or not. this will be useful for Serialization.

class WENGINE_API WSystemsRegister {
public:
    WSystemsRegister();

    WSystemId RegSystem(const char * in_name, const WSystemFn & in_system);

    WSystemFn Get(const WSystemId & in_id) const {
        return system_set_.Get(in_id.GetId());
    }

    WSystemId GetId(const char * in_name) const {
        return name_wid_.at(in_name);
    }

    WSystemId GetId(std::string_view in_name) const {
        return name_wid_.at(std::string(in_name));
    }

    WSystemFn Get(const char * in_name) const {
        return Get(GetId(in_name));
    }

private:

    TSparseSet<WSystemFn> system_set_;

    wcr::IdPool<WSystemId::IdType> id_pool_;
    std::unordered_map<std::string, WSystemId> name_wid_;

};


