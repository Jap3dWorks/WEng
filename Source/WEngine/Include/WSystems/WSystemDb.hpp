#pragma once
#include "WCore/WCore.hpp"
#include "WCore/WIdPool.hpp"
#include "WCore/TSparseSet.hpp"
#include "WSystem.hpp"

#include <vector>
#include <unordered_map>

class WENGINE_API WSystemDb {
private:

    using GlobalSystems = TSparseSet<WSystem::WGlobalSystemFn>;
    using LocalSystems = std::unordered_map<WLevelId, TSparseSet<WSystem::WLocalSystemFn>>;

public:

    void AddGlobalPresystem(WSystem::WGlobalSystemFn);

    void AddGlobalPostsystem(WSystem::WGlobalSystemFn);

    void AddLocalPresystem(const WLevelId &, WSystem::WLocalSystemFn);

    void AddGlobalPresystem(const WLevelId &, WSystem::WLocalSystemFn);

    void RunPresystems(WEngine * in_engine);

    void RunPostsystems(WEngine * in_engine);

    void RunLocalPresystems(WLevel * in_level, WEngine * in_engine);

    void RunLocalPostsystems(WLevel * in_level, WEngine * in_engine);

private:

    GlobalSystems global_presystems_;
    GlobalSystems global_postsystems_;

    LocalSystems local_presystems_;
    LocalSystems local_postsystems_;

};
