#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WEngineObjects/TWAllocator.hpp"
#include "WLog.hpp"

#include <type_traits>
#include <memory>
#include <unordered_map>
#include <typeindex>

class WClass;
class WEntity;
class WComponent;
class WAsset;

struct WDbBuilder {

public:
    
    template<typename B, typename I>
    using DbType = IObjectDataBase<B,I>;

    template<typename B, typename I>
    using DbPtr = std::unique_ptr<DbType<B,I>>;

    template<typename T>
    struct ParamType{
        using Type=T;
    };

    constexpr WDbBuilder()=default;

    virtual ~WDbBuilder()=default;

    constexpr WDbBuilder(const WDbBuilder & other) :
        register_(other.register_) {}

    constexpr WDbBuilder(WDbBuilder && other) noexcept :
        register_(std::move(other.register_)) {}

    constexpr WDbBuilder & operator=(const WDbBuilder & other) {
        if (this != &other) {
            register_ = other.register_;
        }
        return *this;
    }

    constexpr WDbBuilder & operator=(WDbBuilder && other) {
        if (this != &other) {
            register_ = std::move(other.register_);
        }

        return *this;
    }

    template<typename T>
    WDbBuilder(ParamType<T>) {
        RegisterBuilder<T, WAsset, WAssetId>();
        RegisterBuilder<T, WEntity, WEntityId>();
        RegisterBuilder<T, WComponent, WEntityId>();
        
    }

    template<typename B, typename I>
    DbPtr<B,I> Create() const {
        auto erased = CreateDb(typeid(std::pair<B,I>));
        
        return CastDb<B,I>(std::move(erased));
    }

    template<typename T, typename B, typename I>
    void RegisterBuilder() {
        // T should be a derived class of B
        if constexpr (std::is_convertible_v<T*,B*>) {

            if (!register_.contains(typeid(std::pair<B,I>))) {
                register_[typeid(std::pair<B,I>)] = []() -> std::unique_ptr<void*> {
                    return CreateObjectDb<T,B,I>();
                };
            }

            WFLOG("Types already registered! {}, {}", typeid(B).name(), typeid(I).name())

                assert(false);            
        }

    }

private:

    template<typename T, typename B, typename I>
    static inline DbPtr<B,I> CreateObjectDb() {
        TWAllocator<T> a;
        a.SetAllocateFn(
            []
            (T * _pptr, size_t _pn, T* _nptr, size_t _nn) {
                if (_pptr) {
                    for(size_t i=0; i<_pn; i++) {
                        if (!BWRef::IsInstanced(_pptr + i)) {
                            continue;
                        }
                        
                        for (auto & ref : BWRef::Instances(_pptr + i)) {
                            if (ref == nullptr) continue;
                            
                            ref->BSet(_nptr + i);
                        }
                    }
                }
            }
            );

        return std::make_unique<TObjectDataBase<T,
                                                B,
                                                I,
                                                TWAllocator<T>>>(a);
    }

    std::unique_ptr<void*> CreateDb(std::type_index typeindex) const {
        assert(regiter_.contains(typeindex));
        
        return register_.at(typeindex)();
    }

    template<typename B, typename I>
    DbPtr<B,I> CastDb(DbPtr<void, void> && in_cast) {
        return DbPtr<B,I>(static_cast<DbType<B,I>*>(in_cast.release()));
    }

    std::unordered_map<std::type_index, TFunction< std::unique_ptr<void*>() >> register_;

};
