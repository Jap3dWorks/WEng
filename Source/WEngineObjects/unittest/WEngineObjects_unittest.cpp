#include "WAssets/WTextureAsset.hpp"
#include "WCore/TObjectDataBase.hpp"
#include "WCore/WCore.hpp"

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "WCore/TRef.hpp"
// #include "WCore/TFunction.hpp"
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

    auto allocfn = [](size_t* prev, size_t pn, size_t * ptr, size_t n) -> void {
        WLOG("Allocating {} size from {} to {}!\n", n, (size_t)prev, (size_t)ptr);
        prev = ptr;
    };

    auto deallocfn = [](size_t * ptr, size_t n){
        for(size_t i=0; i<n; i++) {
            WLOG("{:d}", (size_t)ptr[i]);
        }
    };

    TWAllocator<size_t, decltype(allocfn), decltype(deallocfn)> a;

    a.SetAllocateFn(allocfn);

    a.SetDeallocateFn(deallocfn);

    std::vector<size_t, TWAllocator<size_t, decltype(allocfn), decltype(deallocfn)>> v{a};

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

    man.CreateAt<WStaticMeshAsset>(1);
    TWRef<WStaticMeshAsset> a =
        man.Get<WStaticMeshAsset>(1);

    man.CreateAt<WTextureAsset>(2);
    TWRef<WTextureAsset> t =
        man.Get<WTextureAsset>(2);

    a->Set_name("a");
    t->Set_name("t");

    WFLOG("Initial \"a\" ptr to: {:d}" , (size_t)a.BPtr());
    WFLOG("Name: {}", a->Get_name().View());

    void* ptr = a.BPtr();

    for (size_t i=0; i<10; i++) {
        man.CreateAt<WStaticMeshAsset>(i+3);
    }

    WFLOG("Final \"a\" ptr to: {:d}", (size_t)a.BPtr());
    WFLOG("Name: {}", a->Get_name().View());

    WFLOG("END")

    return ptr != a.BPtr();
}

bool WObjectDb_WClass_Test() {
    WFLOG("Start");
    
    WObjectDb<WEntity, WEntityId> man;

    WFLOG("Create a1");
    man.CreateAt<WEntity>(1);
    TWRef<WEntity> a1 = man.Get<WEntity>(1);
    
    WFLOG("Create a2");
    man.CreateAt(WEntity::StaticClass(), 2);
    TWRef<WObject> a2 = man.Get(WEntity::StaticClass(),
                                2);

    return a2->Class()->Name() == "WEntity";
}

bool WEntityComponentDb_Test() {

    std::unique_ptr<IObjectDataBase<WComponent, WEntityId::IdType>> smdb =
        WStaticMeshComponent::StaticClass()
        ->DbBuilder()
        .Create<WComponent, WEntityId::IdType>();

    std::unique_ptr<IObjectDataBase<WComponent, WEntityId::IdType>> tdb =
        WTransformComponent::StaticClass()
        ->DbBuilder()
        .Create<WComponent, WEntityId::IdType>();

    std::unique_ptr<IObjectDataBase<WComponent, WEntityId::IdType>> cdb =
        WCameraComponent::StaticClass()
        ->DbBuilder()
        .Create<WComponent, WEntityId::IdType>();

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
    smdb->BForEach([](WComponent * _cmp) {
        WFLOG("PRINT Orig MStaticMeshComponent.");
    });
    cp->BForEach([](WComponent * _cmp) {
        WFLOG("Print Cloned MStaticMeshComponent.");
    });

    if (smdb->Count() != cp->Count()) return false;

    tdb->BForEach([](WComponent * _cmp) {
        WFLOG("Print Orig MTansformsComponent.");
    });
    auto cp2 = tdb->Clone();
    cp2->BForEach([](WComponent * _cmp) {
        WFLOG("Print Cloned MTransformComponent.");
    });
    
    if (tdb->Count() != cp2->Count()) return false;

    auto cp3 = cdb->Clone();
    cdb->BForEach([](WComponent * _cmp) {
        WFLOG("Print Orig WCameraComponent");
    });
    cp3->BForEach([](WComponent * _cmp) {
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

