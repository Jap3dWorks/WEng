#include "WCollision_tests.hpp"

#include <catch2/catch_test_macros.hpp>


TEST_CASE("WCollision") {
    SECTION("WCollision") {
        CHECK(wcl::utests::test1_CheckAABB_1());
        CHECK(wcl::utests::test2_CheckAABB_2());
        CHECK(wcl::utests::test3_OBB_1());
        CHECK(wcl::utests::test4_OBB_2());
        CHECK(wcl::utests::test5_OBB_3());
        CHECK(wcl::utests::test6_OBB_4());
    }
}
