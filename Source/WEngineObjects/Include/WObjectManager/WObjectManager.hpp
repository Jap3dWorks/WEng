#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WEngineObjects/WObject.hpp"
#include "WEngineObjects/TWRef.hpp"

#include <memory>
#include <unordered_map>
#include <memory>
#include <concepts>

#ifndef WOBJECTMANAGER_INITIAL_MEMORY
#define WOBJECTMANAGER_INITIAL_MEMORY 1024
#endif

/**
 * @brief This class is a container for all WObjectContainer.
 * the manager will be the responsible of creation and storage of all WObject types.
 * Also will assign an unique id to each WObject.
*/
class WObjectManager {

public:

    constexpr WObjectManager() noexcept = default;
    virtual ~WObjectManager() = default;

    // You can create a global instance of WObjectManager

    WObjectManager(const WObjectManager&) = delete;
    
    WObjectManager(WObjectManager && other) noexcept :
        containers_(std::move(other.containers_))
        {}
    
    constexpr WObjectManager & operator=(const WObjectManager&) = delete;

    constexpr WObjectManager & operator=(WObjectManager && other) noexcept {
        Move(std::move(other));
        return *this;
    }
    
public:

    /**
     * @brief Create a new WObject of type T.
     */
    template<std::derived_from<WObject> T>
    TWRef<T> CreateObject(const char * in_fullname) {
        
        WClass * w_class = T::StaticClass();
        return static_cast<T*>(
            CreateObject(w_class, in_fullname).Ptr()
            );

    }

    TWRef<WObject> CreateObject(WClass * in_class, const char * in_fullname) {
        
        if (!containers_.contains(in_class)) {
            containers_[in_class] =
                in_class->CreateObjectDatabase();
            
            containers_[in_class]->Reserve(
                WOBJECTMANAGER_INITIAL_MEMORY
                );
        }

        WId id = containers_[in_class]->Create();
        
        WObject * obj;
        containers_[in_class]->Get(id, obj);

        obj->WID(id);
        obj->Name(in_fullname);

        return obj;

    }

    template <std::derived_from<WObject> T>
    TWRef<T> GetObject(WId in_id) {
        WClass * object_class = T::StaticClass();

        return static_cast<T*>(GetObject(object_class, in_id).Ptr());
    }

    TWRef<WObject> GetObject(WClass * in_class, WId in_id) {
        WObject * result;
        containers_[in_class]->Get(in_id, result);

        return static_cast<WObject*>(result);
    }

    template<std::derived_from<WObject> T>
    void ForEach(TFunction<void(T*)> in_predicate) {
        if (!containers_[T::StaticClass()]) {
            return;
        }

        containers_[T::StaticClass()]->ForEach(
            [&in_predicate](WObject* ptr_) {
                in_predicate(
                    static_cast<T*>(ptr_)
                    );
            }
            );
    }

    void ForEach(WClass * in_class, TFunction<void(WObject*)> in_predicate) {
        containers_[in_class]->ForEach(in_predicate);
    }

private:

    constexpr void Move(WObjectManager && other) noexcept {
        containers_ = std::move(other.containers_);
    }

private:    

    std::unordered_map<WClass *, std::unique_ptr<IObjectDataBase<WObject>>> containers_{};

};

