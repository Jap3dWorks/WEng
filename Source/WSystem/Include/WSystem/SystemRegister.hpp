#pragma once

#include "WCore/WCore.hpp"
#include "WSystem/WSystem.hpp"
#include "WCore/TSparseSet.hpp"
#include "WCore/IdPool.hpp"

#include <unordered_map>
#include <string>

namespace wsm {

    class WENGINE_API SystemsRegister {

    public:

    wcr::wid::WSystemId RegSystem(
        std::string_view in_name,
        wsm::SystemFn const & in_system
        ) {
        wcr::wid::WSystemId id = id_pool_.Generate();

        wid_system_.insert({id.GetId(), in_system});
        name_wid_.insert({in_name, id.GetId()});

        return id;
    }

    wsm::SystemFn GetSystem(wcr::wid::WSystemId const & in_id) const {
        return wid_system_.at(in_id.GetId());
    }

    wcr::wid::WSystemId GetId(std::string_view in_name) const {
        return name_wid_.at(in_name);
    }

    wsm::SystemFn GetSystem(std::string_view in_name) const {
        return GetSystem(GetId(in_name));
    }

    private:

    wcr::IdPool<wcr::wid::WSystemId::IdType> id_pool_{};

    std::unordered_map<wcr::wid::WSystemId::IdType, wsm::SystemFn> wid_system_{};
    std::unordered_map<std::string_view, wcr::wid::WSystemId::IdType> name_wid_{};

    };
}
