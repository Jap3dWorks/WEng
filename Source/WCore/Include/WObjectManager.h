#pragma once

#include "WCore/WCore.h"
#include <vector>
#include <memory>

/*
 * This class is a container for all WObjects of a specific type.
 * Each WObject that is created will be added to the appropriate WObjectTypeContainer.
 * We should create a WObjectTypeContainer for each type of WObject using reflection system.
*/
class WCORE_API WObjectTypeContainer
{
public:
    WObjectTypeContainer() = default;
    virtual ~WObjectTypeContainer() = default;

    virtual WClass GetObjectType() const = 0;
};

/*
 * This class is a container for all WObjectTypeContainer.
 * the manager will be the responsible of creation and storage of all WObject types.
 * Also will assign an unique id to each WObject.
*/
class WCORE_API WObjectManager
{
public:
    WObjectManager() = default;
    virtual ~WObjectManager()
    {
        // Remove all objects
        // objects_.clear();

    }

private:

    // 1st dimension is the container of an specific type of WObject
    std::vector<std::unique_ptr<WObjectTypeContainer>> objects_{};

    template <typename T>
    WObject* CreateObject(const char* object_name, T* null=nullptr)
    {
        // T::
    }

    // virtual void AddObject(WObject* object) = 0;
    // virtual void RemoveObject(WObject* object) = 0;
    // virtual void RemoveObject(const char* object_name) = 0;
    // virtual void RemoveAllObjects() = 0;

};
