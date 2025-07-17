#include "WEngineObjects/WObjectMacros.hpp"
#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "WCore/TRef.hpp"
#include "WCore/TFunction.hpp"
#include "WEngineObjects/TWAllocator.hpp"
#include "WObjectManager/WObjectManager.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WEngineObjects/WActor.hpp"
#include "WLog.hpp"

#include <vector>
#include <cstdio>

bool TWAllocator_in_vector() {
    WFLOG("START")

    TWAllocator<size_t> a;

    a.SetAllocateFn([](size_t* prev, size_t pn, size_t * ptr, size_t n) -> void {
        WLOG("Allocating {} size from {} to {}!\n", n, (size_t)prev, (size_t)ptr);
        prev = ptr;
    });

    a.SetDeallocateFn([](size_t * ptr, size_t n){
        for(size_t i=0; i<n; i++) {
            WLOG("{:d}", (size_t)ptr[i]);
        }
    });

    std::vector<size_t, TWAllocator<size_t>> v{a};

    for (size_t i=0; i<10; i++) {
        v.push_back(i);
    }

    WFLOG("");

    for (auto& n : v) {
        WFLOG("{:d}", n);
    }
    
    WFLOG("END");

    return true;
}

bool WClass_Derived_Test() {
    WFLOG("START")
        
    const WClass * cls1 = WObject::StaticClass();
    const WClass * cls2 = WObject::StaticClass();

    const WClass * cls3 = WActor::StaticClass();

    WFLOG("WObject is base of WObject: {}", cls1->IsBaseOf(cls2));

    WFLOG("END")

    return cls1 == cls2 && cls1->IsBaseOf(cls3);
}

bool WObjectManager_TWRef_Test() {
    WFLOG("START")
    WObjectManager man;

    man.InitialMemorySize(1);

    TWRef<WStaticMeshAsset> a  = man.CreateObject<WStaticMeshAsset>("a");
    a->Name("a");

    WFLOG("Initial \"a\" ptr to: {:d}" , (size_t)a.BPtr());
    WFLOG("Name: {}", a->Name().c_str());

    void* ptr = a.BPtr();

    for (size_t i=0; i<10; i++) {
        auto z = man.CreateObject<WStaticMeshAsset>("z");
    }

    WFLOG("Final \"a\" ptr to: {:d}", (size_t)a.BPtr());
    WFLOG("Name: {}", a->Name().c_str());

    WFLOG("END")

    return ptr != a.BPtr();
}

bool WObjectManager_WClass_Test() {
    WFLOG("Start");
    
    WObjectManager man;

    WFLOG("Create a1");
    TWRef<WActor> a1 = man.CreateObject<WActor>("/Content/a1.a1");
    WFLOG("Create a2");
    TWRef<WObject> a2 = man.CreateObject(WActor::StaticClass(), "/Content/a2.a2");

    WFLOG("{} is a {}", a1->Name(), a1->Class()->Name());
    WFLOG("{} is a {}", a2->Name(), a2->Class()->Name());

    WFLOG("PRINT ACTORS TEMPLATE:");

    man.ForEach<WActor>([](WActor* in){ WLOG("{}", in->Name());});

    WFLOG("PRINT ACTORS WCLASS:");

    man.ForEach(WActor::StaticClass(), [](WObject* in){ WLOG("{}", in->Class()->Name());});

    WFLOG("End");

    return a2->Class()->Name() == "WActor";
}

TEST_CASE("WEngineObjects") {
    SECTION("TWAllocator") {
        CHECK(TWAllocator_in_vector());
    }
    SECTION("WClass") {
        CHECK(WClass_Derived_Test());
    }
    SECTION("WObjectManager") {
        CHECK(WObjectManager_TWRef_Test());
        CHECK(WObjectManager_WClass_Test());
    }
}

