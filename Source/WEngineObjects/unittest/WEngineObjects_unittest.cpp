#include "WAssets/WTextureAsset.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WCore/WCore.hpp"
#include "WEngineObjects/WObjectMacros.hpp"
#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "WCore/TRef.hpp"
#include "WCore/TFunction.hpp"
#include "WCore/TWAllocator.hpp"
#include "WObjectDb/WObjectDb.hpp"
#include "WEngineObjects/WAsset.hpp"
#include "WAssets/WStaticMeshAsset.hpp"
#include "WEngineObjects/WEntity.hpp"
#include "WObjectDb/WEntityComponentDb.hpp"
#include "WComponents/WStaticMeshComponent.hpp"
#include "WComponents/WTransformComponent.hpp"
#include "WComponents/WCameraComponent.hpp"

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

    const WClass * cls3 = WEntity::StaticClass();

    WFLOG("WObject is base of WObject: {}", cls1->IsBaseOf(cls2));

    WFLOG("END")

    return cls1 == cls2 && cls1->IsBaseOf(cls3);
}

bool WObjectDb_TWRef_Test() {
    WFLOG("START");
    WObjectDb<WAsset, WAssetId> man;

    man.InitialMemorySize(1);

    TWRef<WStaticMeshAsset> a = man.Get<WStaticMeshAsset>(man.Create<WStaticMeshAsset>());
    TWRef<WTextureAsset> t = man.Get<WTextureAsset>(man.Create<WTextureAsset>());

    a->Name("a");
    t->Name("t");

    WFLOG("Initial \"a\" ptr to: {:d}" , (size_t)a.BPtr());
    WFLOG("Name: {}", a->Name());

    void* ptr = a.BPtr();

    for (size_t i=0; i<10; i++) {
        auto z = man.Create<WStaticMeshAsset>();
    }

    WFLOG("Final \"a\" ptr to: {:d}", (size_t)a.BPtr());
    WFLOG("Name: {}", a->Name());

    WFLOG("END")

    return ptr != a.BPtr();
}

bool WObjectDb_WClass_Test() {
    WFLOG("Start");
    
    WObjectDb<WEntity, WEntityId> man;

    WFLOG("Create a1");
    TWRef<WEntity> a1 = man.Get<WEntity>(man.Create<WEntity>());
    
    WFLOG("Create a2");
    TWRef<WObject> a2 = man.Get(WEntity::StaticClass(),
                                man.Create(WEntity::StaticClass()));

    return a2->Class()->Name() == "WEntity";
}

bool WEntityComponentDb_Test() {

    std::unique_ptr<IObjectDataBase<WComponent, WEntityId>> smdb =
        WStaticMeshComponent::StaticClass()
        ->DbBuilder()
        .Create<WComponent, WEntityId>();

    std::unique_ptr<IObjectDataBase<WComponent, WEntityId>> tdb =
        WTransformComponent::StaticClass()
        ->DbBuilder()
        .Create<WComponent, WEntityId>();

    std::unique_ptr<IObjectDataBase<WComponent, WEntityId>> cdb =
        WCameraComponent::StaticClass()
        ->DbBuilder()
        .Create<WComponent, WEntityId>();

    smdb->Create();
    smdb->Create();
    smdb->Create();

    tdb->Create();
    tdb->Create();
    tdb->Create();

    cdb->Create();
    cdb->Create();
    cdb->Create();

    auto cp = smdb->Clone();
    smdb->ForEach([](WComponent * _cmp) {
        WFLOG("PRINT Orig MStaticMeshComponent.");
    });
    cp->ForEach([](WComponent * _cmp) {
        WFLOG("Print Cloned MStaticMeshComponent.");
    });

    if (smdb->Count() != cp->Count()) return false;

    tdb->ForEach([](WComponent * _cmp) {
        WFLOG("Print Orig MTansformsComponent.");
    });
    auto cp2 = tdb->Clone();
    cp2->ForEach([](WComponent * _cmp) {
        WFLOG("Print Cloned MTransformComponent.");
    });
    
    if (tdb->Count() != cp2->Count()) return false;

    auto cp3 = cdb->Clone();
    cdb->ForEach([](WComponent * _cmp) {
        WFLOG("Print Orig WCameraComponent");
    });
    cp3->ForEach([](WComponent * _cmp) {
        WFLOG("Print Cloned WCameraComponent");
    });

    if (cdb->Count() != cp3->Count()) return false;

    WEntityComponentDb db;

    WEntityId eid = db.CreateEntity<WEntity>("E1");

    db.CreateComponent<WTransformComponent>(eid);
    db.CreateComponent<WCameraComponent>(eid);
    db.CreateComponent<WStaticMeshComponent>(eid);

    WEntityComponentDb other = db;
    
    return true;
}

TEST_CASE("WEngineObjects") {
    SECTION("TWAllocator") {
        CHECK(TWAllocator_in_vector());
    }
    SECTION("WClass") {
        CHECK(WClass_Derived_Test());
    }
    SECTION("WObjectDb") {
        CHECK(WObjectDb_TWRef_Test());
        CHECK(WObjectDb_WClass_Test());
    }
    SECTION("WEntityComponentDb") {
        CHECK(WEntityComponentDb_Test());
    }
}

