#pragma once

#include "WCore/WCore.h"
#include "WCore/WIdPool.h"
#include <vector>

// Manage the aquirement and release of texture or mesh buffer and stuffs like that.

template<typename T>
class WRENDER_API WVkAssetCollection
{
public:

    WVkAssetCollection();

    ~WVkAssetCollection();

    WVkAssetCollection(const WVkAssetCollection<T> & other);

    WVkAssetCollection(WVkAssetCollection<T> && other);

    WVkAssetCollection<T> & operator=(const WVkAssetCollection<T> & other);

    WVkAssetCollection<T> & operator=(WVkAssetCollection<T> && other);

    void AddAsset(const T & other);

private:

    std::vector<T> data_{};

    WIdPool id_pool_{};
    
};
