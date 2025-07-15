#pragma once

#include "WEngineObjects/WClass.hpp"

/**
 * @brief WClass declaration for an WObject.
 */
template<typename T>
class WClassFor : public WClass {
    
public:

    WClassFor() : WClass(typeid(T).name()) {}
    ~WClassFor() override = default;

public:

    std::unique_ptr<IObjectDataBase> CreateObjectDatabase() override {
        TWAllocator<T> a;

        a.SetAllocateFn(
            []
            (T *& prev, T* nptr, size_t n) {
                if (prev) {
                    for(size_t i=0; i<n; i++) {
                        if (!BWRef::IsInstanced(prev + i)) {
                            continue;
                        }
                        
                        for (auto & ref : BWRef::Instances(prev + i)) {
                            if (ref == nullptr) continue;
                            
                            ref->BSet(nptr + i);
                        }
                    }
                }
            });

        return std::make_unique<TObjectDataBase<T, TWAllocator<T>>>(a);

    }

    const WObject * DefaultObject() const override {
        return &T::GetDefaultObject();
    }
    
};
