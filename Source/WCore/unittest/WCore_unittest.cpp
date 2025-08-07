#include "WCore/TObjectDataBase.hpp"
#include "WCore/WCore.hpp"

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

bool TObjectDataBase_Test() {

    TObjectDataBase<A, B, WId> od;

    od.Insert(1);
    od.Insert(2);
    od.Insert(3);

    TObjectDataBase<A, B, WId> od2 = od;

    std::unique_ptr<IObjectDataBase<B,WId>>od3 = od.Clone();

    od3->ForEach([](B * b) -> void {
        std::print("Print od3 Item\n.");
    });

    return od3->Count() == od.Count() && od.Count() == od2.Count();

}

TEST_CASE("WCore") {
    SECTION("TObjectDataBase") {
        CHECK(TObjectDataBase_Test());
    }
}

