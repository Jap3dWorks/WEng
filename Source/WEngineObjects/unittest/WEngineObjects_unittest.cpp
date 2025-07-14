#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "WCore/TRef.hpp"
#include "WCore/TFunction.hpp"
#include "WEngineObjects/TWAllocator.hpp"
#include "WObjectManager/WObjectManager.hpp"
#include "WAssets/WStaticMeshAsset.hpp"

#include <vector>
#include <cstdio>

#ifdef WOBJECTMANAGER_INITIAL_MEMORY
    #undef WOBJECTMANAGER_INITIAL_MEMORY
#endif

#define WOBJECTMANAGER_INITIAL_MEMORY 1


bool TWAllocator_in_vector() {

    TWAllocator<size_t> a;

    size_t * prev = {nullptr};

    a.SetAllocateFn([&prev](size_t * ptr, size_t n) {
        std::printf("Allocating %d size from %d to %d!\n", n, prev, ptr);
        prev = ptr;
    });

    a.SetDeallocateFn([&prev](size_t * ptr, size_t n){
        std::printf("Deallocating %d size from %d!\n", n, ptr);
        std::printf("  Fix Elements from %d\n", prev);
        std::printf("  Prev elenments:\n   ");
        for(size_t i=0; i<n; i++) {
            // prev[i]++;
            std::printf("%d, ", ptr[i]);
        }
        std::printf("\n");
        std::printf("  New elements:\n   ");
        for(size_t i=0; i<n; i++) {
            std::printf("%d, ", prev[i]);
            prev[i]++;
        }
        std::printf("\n");
    });

    std::vector<size_t, TWAllocator<size_t>> v{a};

    for (size_t i=0; i<10; i++) {
        v.push_back(i);
    }

    std::printf("\n");

    for (auto& n : v) {
        std::printf("%d, ", n);
    }
    
    std::printf("\n");

    return true;
}

bool WObjectManager_TWRef_test() {
    WObjectManager man;

    TWRef<WStaticMeshAsset> a  = man.CreateObject<WStaticMeshAsset>("a");
    a->Name("a");

    std::printf("Initial \"a\" ptr to: %d\n" , a.BPtr());
    std::printf("Name: %s\n", a->Name().c_str());

    void* ptr = a.BPtr();

    for (size_t i=0; i<10; i++) {
        auto z = man.CreateObject<WStaticMeshAsset>("z");
    }

    std::printf("Final \"a\" ptr to: %d\n", a.BPtr());
    std::printf("Name: %s\n", a->Name().c_str());

    return ptr != a.BPtr();
}

TEST_CASE("WEngineObjects") {
    SECTION("TWAllocator") {
        CHECK(TWAllocator_in_vector());
    }
    SECTION("WOBjectManager") {
        CHECK(WObjectManager_TWRef_test());
    }
    
}

