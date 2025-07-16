#pragma once

#include "WEngineObjects/WClass.hpp"
#include "WEngineObjects/TWRef.hpp"
#include "WLog.hpp"

/**
 * @brief WClass declaration for an WObject.
 */
template<typename T>
class WClassFor : public WClass {
    
public:

    WClassFor() : WClass(typeid(T).name()) {}
    ~WClassFor() override = default;

public:

    std::unique_ptr<IObjectDataBase<WObject>> CreateObjectDatabase() override {
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
            });

        return std::make_unique<TObjectDataBase<T, WObject, TWAllocator<T>>>(a);

    }

    const WObject * DefaultObject() const override {
        return &T::DefaultObject();
    }
    
};
