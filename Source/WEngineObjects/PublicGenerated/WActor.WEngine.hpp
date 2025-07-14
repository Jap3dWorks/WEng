#pragma once

#include "WCore/WCore.hpp"
#include "WEngineObjects/WClass.hpp"
#include "WEngineObjects/TWAllocator.hpp"
#include "WCore/TObjectDataBase.hpp"

class WActor;

class WENGINEOBJECTS_API WClass__WActor : public WClass{
public :

    constexpr WClass__WActor() :
        WClass("WClass__WActor") {}

    ~WClass__WActor() override = default;

public:

    std::unique_ptr<IObjectDataBase> CreateObjectDatabase() override;

    WObject * DefaultObject() const override;


    // std::unique_ptr<IObjectDataBase> CreateObjectDatabase() override {
    //     TWAllocator<WActor> a;
    //     a.SetAllocateFn(
    //         [prev = (WActor*)(nullptr)]
    //         (WActor* ptr, size_t n) mutable {
    //             if (prev) {
    //                 for(size_t i=0; i<n; i++) {
    //                     if (!BWRef::IsInstanced(prev + i)) {
    //                         continue;
    //                     }
                        
    //                     for (auto & ref : BWRef::Instances(prev + i)) {
    //                         if (ref == nullptr) continue;
                            
    //                         ref->BSet(ptr + i);
    //                     }
    //                 }
    //             }

    //             prev = ptr;
    //         });

    //     std::make_unique<TObjectDataBase<WActor, TWAllocator<WActor>>>(
    //         CreateAllocator<WActor>()
    //         );
        
        // return nullptr;
    // }

};
