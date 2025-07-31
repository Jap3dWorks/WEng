#pragma once

#include "WCore/WConcepts.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WEngineObjects/TWAllocator.hpp"
#include "WLog.hpp"

#include <memory>
#include <unordered_map>
#include <typeindex>

struct WDbBuilder {

public:
    
    template<typename B, typename I>
    using DbType = IObjectDataBase<B,I>;

    template<typename B, typename I>
    using DbRawPtr = DbType<B,I>*;

    using VoidPtr = void*;

    template<typename B, typename I>
    using RegKey = std::pair<B,I>;

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

    template<typename B, typename I>
    std::unique_ptr<DbType<B,I>> Create() const {
        VoidPtr type_erased = CreateDb(typeid(RegKey<B,I>));
        
        return CastDb<B,I>(type_erased);
    }

    template<typename T, CConvertibleTo<T> B, typename I>
    void RegisterBuilder() {
        if (!register_.contains(typeid(RegKey<B,I>))) {
            
            // register_[typeid(RegKey<B,I>)] = []() -> VoidPtr {
            //     return CreateObjectDb<T,B,I>();
            // };

            register_[typeid(RegKey<B,I>)] = &CreateObjectDb<T,B,I>;
            
        }

        WFLOG("Types already registered! {}, {}", typeid(B).name(), typeid(I).name());

        // assert(false);            
    }

private:

    template<typename T, CConvertibleTo<T> B, typename I>
    static inline VoidPtr CreateObjectDb() {
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

        return  new TObjectDataBase<T,
                                    B,
                                    I,
                                    TWAllocator<T>>(a);
    }

    VoidPtr CreateDb(std::type_index typeindex) const {
        assert(register_.contains(typeindex));
        
        return register_.at(typeindex)();
    }

    template<typename B, typename I>
    DbRawPtr<B,I> CastDb(VoidPtr in_cast) {
        return static_cast<DbRawPtr<B,I>>(in_cast);
    }

    std::unordered_map<std::type_index, TFunction< VoidPtr() >> register_;

};
