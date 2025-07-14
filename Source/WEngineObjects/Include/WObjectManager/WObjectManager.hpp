#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WEngineObjects/TWAllocator.hpp"
#include "WEngineObjects/WObject.hpp"
#include "WEngineObjects/TWRef.hpp"

#include <vector>
#include <memory>
#include <unordered_map>
#include <array>
#include <memory>
#include <type_traits>
#include <concepts>

#ifndef WOBJECTMANAGER_INITIAL_MEMORY
#define WOBJECTMANAGER_INITIAL_MEMORY 1024
#endif

/*
 * This class is a container for all WObjectContainer.
 * the manager will be the responsible of creation and storage of all WObject types.
 * Also will assign an unique id to each WObject.
*/
class WObjectManager
{

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

    // TODO implement TWRef reference track wrappers

    /**
     * @brief Create a new WObject of type T
    */
    template <std::derived_from<WObject> T>
    TWRef<T> CreateObject(const char* in_object_path) {
        WClass * object_class = T::GetStaticClass();

        if (!containers_.contains(*object_class)) {
            containers_[*object_class] =
                std::make_unique<TObjectDataBase<T, TWAllocator<T>>>(
                    CreateAllocator<T>()
                    );
            
            containers_[*object_class]->Reserve(
                WOBJECTMANAGER_INITIAL_MEMORY
                );
        }

        WId id = containers_[*object_class]->Create();

        void* result;

        containers_[*object_class]->Get(id, result);

        T* asset = reinterpret_cast<T*>(result);
        asset->WID(id);

        return asset;
    }

    // WId CreateObject(const WClass & in_object_class, const char* in_object_path) {
    //     if (!containers_.contains(in_object_class)) {
            
    //         containers_[in_object_class] =
    //             std::make_unique<TObjectDataBase<T, TWAllocator<T>>>(
    //                 CreateAllocator<T>()
    //                 );
            
    //         containers_[in_object_class]->Reserve(
    //             WOBJECTMANAGER_INITIAL_MEMORY
    //             );
    //     }

    //     WId id = containers_[in_object_class]->Create();

    //     return id;
    // }

    template <std::derived_from<WObject> T>
    TWRef<T> GetObject(WId in_id)
    {
        WClass * object_class = T::GetDefaultObject()->GetClass();

        void * result;
        containers_[*object_class]->Get(in_id, result);

        return reinterpret_cast<T*>(result);
    }

private:

    template<std::derived_from<WObject> T>
    TWAllocator<T> CreateAllocator() const {
        TWAllocator<T> a;
        
        a.SetAllocateFn(
            []
            (T* ptr, size_t n) {
                if (PtrTrack<T>::Get()) {
                    for(size_t i=0; i<n; i++) {
                        if (!BWRef::IsInstanced(PtrTrack<T>::Get() + i)) {
                            continue;
                        }
                        
                        for (auto & ref : BWRef::Instances(PtrTrack<T>::Get() + i)) {
                            if (ref == nullptr) continue;
                            
                            ref->BSet(ptr + i);
                        }
                    }
                }
                
                PtrTrack<T>::Set(ptr);
            });

        return a;
    }

    template<std::derived_from<WObject> T>
    struct PtrTrack {
        static T* Get() {
            return PtrTrack<T>::ptr_;
        }

        static void Set(T* in_ptr) {
            PtrTrack<T>::ptr_ = in_ptr;
        }

    private:
        
        static inline T* ptr_{nullptr};
    };

    constexpr void Move(WObjectManager && other) noexcept {
        containers_ = std::move(other.containers_);
    }

private:    

    std::unordered_map<WClass, std::unique_ptr<IObjectDataBase>> containers_{};

};

