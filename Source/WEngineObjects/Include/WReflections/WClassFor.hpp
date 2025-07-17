#pragma once

#include "WEngineObjects/WClass.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WLog.hpp"
#include <concepts>
#include <type_traits>

#ifndef WCLASSFOR_N_SIZE
#define WCLASSFOR_N_SIZE 32
#endif

class WObject;

/**
 * @brief WClass declaration for an WObject.
 */
template<typename T, const char N[WCLASSFOR_N_SIZE], std::derived_from<WObject> P=WObject>
class WClassFor : public WClass {
    
public:

    constexpr WClassFor() noexcept :
        WClass(N) {}
    
public:

    std::unique_ptr<IObjectDataBase<WObject>> CreateObjectDatabase() const override {
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

        return std::make_unique<TObjectDataBase<T, WObject, TWAllocator<T>>>(a);

    }

    constexpr const WClass * BaseClass() const override {
        return BaseClassConstexpr();
    }

    constexpr const WClass * BaseClassConstexpr() const {
        if constexpr (std::is_same_v<T, WObject>) {
            return nullptr;
        }
        else {
            return P::StaticClass();
        }
    }

    constexpr std::unordered_set<const WClass*> Bases() const {
        return BasesConstexpr();
    }

    constexpr std::unordered_set<const WClass*> BasesConstexpr() const {
        std::unordered_set<const WClass*> result;
        const WClass* b = BaseClassConstexpr();
        while(b) {
            result.insert(b);             // I think this is constexpr in c++23 
            b = b->BaseClass();           // in c++23 virtual methods can be constexpr
        }

        return result;
    }

};
