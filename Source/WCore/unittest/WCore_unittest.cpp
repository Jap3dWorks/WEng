#include "WCore/TObjectDataBase.hpp"
#include "WCore/WCore.hpp"
#include "WCore/TWAllocator.hpp"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "WCore/TRef.hpp"
#include "WCore/TFunction.hpp"
#include "WLog.hpp"

#include <vector>
#include <cstdio>
#include <print>

struct B{};

struct A : public B {};

bool TWAllocator_1_Test() {
    WFLOG("--Init Test--");

    struct A {
        A(std::size_t in_a) : a(in_a) {}

       ~A() { WFLOG("Destroy A \"{}\"!", a); }

        A(const A&)=default;
        A(A&&)=default;
        A & operator=(const A&)=default;
        A & operator=(A &&)=default;
    private:
        std::size_t a;
    };

    auto allocfn = [](A*, std::size_t, A*, std::size_t){};
    auto deallfn = [](A*, std::size_t){};

    std::vector<A, TWAllocator<A, decltype(allocfn), decltype(deallfn)>> va;

    for (std::uint32_t i=0; i<8; i++) {
        va.push_back(A(i));
    }

    va.clear();

    va={};
    
    return true;
}

bool TObjectDataBase_Test() {
    WFLOG("--Init Test--");
    
    TObjectDataBase<A, B, WId> od;

    od.CreateAt(1);
    od.CreateAt(2);
    od.CreateAt(3);

    TObjectDataBase<A, B, WId> od2 = od;

    std::unique_ptr<IObjectDataBase<B,WId>>od3 = od.Clone();

    od3->ForEach([](B * b) -> void {
        WFLOG("Print od3 Item.");
    });

    return od3->Count() == od.Count() && od.Count() == od2.Count();

}

TEST_CASE("WCore") {
    SECTION("TWAllocator") {
        CHECK(TWAllocator_1_Test());
    }
    SECTION("TObjectDataBase") {
        CHECK(TObjectDataBase_Test());
    }
}

