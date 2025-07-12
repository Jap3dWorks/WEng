#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "WCore/TFunction.hpp"
#include "WEngineObjects/TWAllocator.hpp"
#include "WCore/TRef.hpp"

#include <vector>
#include <cstdio>

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

TEST_CASE("TWAllocator") {
    SECTION("T1") {
        CHECK(TWAllocator_in_vector());
    }
    
}

