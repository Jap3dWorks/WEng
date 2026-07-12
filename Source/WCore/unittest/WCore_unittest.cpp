#include "WCore/TObjectDataBase.hpp"
#include "WCore/WCore.hpp"
#include "WCore/TWAllocator.hpp"
#include "WCore/WId.hpp"
#include <functional>
#include <string_view>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "WCore/TRef.hpp"
#include "WString/WString.hpp"

#include "WLog.hpp"

#include <vector>
#include <cstdio>
#include <cstdint>
#include <print>
#include <bitset> 

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

    WFLOG("-- Init TObjectDataBase_Test --");
    
    TObjectDataBase<A, B, WId<>::IdType> od{
        [](WId<>::IdType const &) ->A {return {};},
        [](A const & )->void {}
    };

    od.InsertAt<A>(1, {});
    od.InsertAt<A>(2, {});
    od.InsertAt<A>(3, {});

    WFLOG("od Count: {}", od.Count());

    TObjectDataBase<A, B, WId<>::IdType> od2 = od;

    WFLOG("od2 Count: {}", od2.Count());

    std::unique_ptr<IObjectDataBase<B,WId<>::IdType>>od3 = od.Clone();

    WFLOG("od3 Count: {}", od3->Count());

    od3->BForEach([](B * b) -> void {
        WFLOG("Print od3 Item.");
    });

    return od3->Count() == od.Count() && od.Count() == od2.Count();

}

bool SplitAssetPath_Test() {

    WFLOG("-- SplitAssetPath test --");

    std::vector<std::string_view> expected {
        "Content", "Assets", "Textures", "texture01", "texture01"
    };

    std::string_view s = "Content/Assets/Textures\\texture01:texture01";

    auto result = wstr::SplitAssetPath(s);

    WFLOG("Split path result: {}", result);
    WFLOG("Expected result: {}", expected);

    return expected == result;

}

bool WIDCompoundNullValue_Test() {
    WTypeAssetIndexId wid{WID_NULL_V};

    auto idvalue = wid.GetId();

    std::bitset<64> bt {idvalue};

    std::cout << "Value: " << bt <<std::endl;
    
    return true;
}


TEST_CASE("WCore") {
    SECTION("TWAllocator") {
        CHECK(TWAllocator_1_Test());
    }
    SECTION("TObjectDataBase") {
        CHECK(TObjectDataBase_Test());
    }
    SECTION("WString") {
        CHECK(SplitAssetPath_Test());
    }
    SECTION("WId") {
        CHECK(WIDCompoundNullValue_Test());
    }

}

