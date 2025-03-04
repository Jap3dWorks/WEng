#pragma once

#include "WCore/WCore.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <array>
#include <memory>
#include <type_traits>
#include <concepts>


#define WCONTAINER_MAX_OBJECTS 1024

/*
 * This class is a container for all WObjects of a specific type.
 * Each WObject that is created will be added to the appropriate WObjectContainer<T>.
*/
namespace detail
{
    class WCORE_API WObjectContainerBase
    {
    public:
        WObjectContainerBase() = default;
        virtual ~WObjectContainerBase() = default;

        virtual WClass GetObjectType() const = 0;
        virtual WObject* CreateObject() = 0;
        virtual WObject* GetObject(WId id) = 0;

        size_t GetSize() const;

        // iterator
        class Iterator
        {
        public:
            Iterator() = delete;
            Iterator(WObject* object);

            virtual ~Iterator() = default;

            WObject& operator*();
            WObject* operator->();
            Iterator& operator++();
            Iterator operator++(int);
            bool operator==(const Iterator& other);
            bool operator!=(const Iterator& other);

        private:
            WObject* object_{};

        };

        virtual Iterator begin() = 0;
        virtual Iterator end() = 0;

    protected:

        size_t size_{0};
        void AssignNewId(WObject* object);
        
    };

    template <typename T, size_t max_size=WCONTAINER_MAX_OBJECTS>
    class WObjectContainer : public detail::WObjectContainerBase
    {
    public:
        WObjectContainer() = default;
        virtual ~WObjectContainer() = default;

        WClass GetObjectType() const override
        {
            return T::GetDefaultObject().GetClass();
        }

        WObject* CreateObject() override
        {
            T& object = objects_[size_];
            AssignNewId(&object);

            return &object;
        }

        WObject* GetObject(WId id) override
        {
            return &objects_[id];
        }

        virtual Iterator begin()
        {
            return Iterator{&objects_[0]};
        }
        virtual Iterator end()
        {
            if (size_ == objects_.size())
            {
                return Iterator{(&objects_[size_-1]) + 1};
            }
            else
            {
                return Iterator{&objects_[size_]};
            }
        }

    private:
        // by now use a fixed size array, in the future we should use a dynamic array
        // we use fixes to avoid dynamic memory allocation.
        std::array<T, max_size> objects_{};
    };
}


/*
 * This class is a container for all WObjectContainer.
 * the manager will be the responsible of creation and storage of all WObject types.
 * Also will assign an unique id to each WObject.
*/
class WCORE_API WObjectManager
{
public:
    WObjectManager() = default;
    virtual ~WObjectManager() = default;

    // You can create a global instance of WObjectManager
    static WObjectManager& GetInstance();

    WObjectManager(const WObjectManager&) = delete;
    WObjectManager& operator=(const WObjectManager&) = delete;

private:

    std::unordered_map<WClass, std::unique_ptr<detail::WObjectContainerBase>> containers_{};

public:
    /**
     * @brief Create a new WObject of type T
    */
    template <std::derived_from<WObject> T>
    T* CreateObject(const char* object_path)
    {
        const WClass& object_class = T::GetStaticClass();

        if (!containers_.contains(object_class))
        {
            containers_[object_class] = std::make_unique<detail::WObjectContainer<T>>();
        }

        return reinterpret_cast<T*>(containers_[object_class]->CreateObject());
    }

    template <std::derived_from<WObject> T>
    T* GetObject(WId id)
    {
        WClass object_class = T::GetDefaultObject()->GetClass();
        return reinterpret_cast<T*>(containers_[object_class]->GetObject(id));
    }
};

