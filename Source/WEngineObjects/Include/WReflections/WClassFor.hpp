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

    WDbBuilder DbBuilder() const override {
        WDbBuilder r;

        if constexpr (std::is_convertible_v<T*,WAsset*>) {
            r.RegisterBuilder<T, WAsset, WAssetId>();
        }
        if constexpr (std::is_convertible_v<T*,WEntity*>) {
            r.RegisterBuilder<T, WEntity, WEntityId>();
        }
        if constexpr(std::is_convertible_v<T*, WComponent*>) {
            r.RegisterBuilder<T, WComponent, WEntityId>();
        }

        return r;
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

    constexpr std::unordered_set<const WClass*> Bases() const override {
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

protected:

private:

};
