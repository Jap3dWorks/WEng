#include "WCollision_tests.hpp"

#include <catch2/catch_test_macros.hpp>


TEST_CASE("WCollision") {
    SECTION("WCollision") {
        CHECK(wcl::utests::test_CheckAABB_1());
        CHECK(wcl::utests::test_CheckAABB_2());
        CHECK(wcl::utests::test_BOX_1());
        CHECK(wcl::utests::test_BOX_2());
        CHECK(wcl::utests::test_BOX_3());
        CHECK(wcl::utests::test_BOX_4());
        CHECK(wcl::utests::test_BOX_capsule_1());
        CHECK(wcl::utests::test_BOX_capsule_2());
        CHECK(wcl::utests::test_BOX_capsule_3());
        CHECK(wcl::utests::test_BOX_capsule_4());
        CHECK(wcl::utests::test_sphere_1());
        CHECK(wcl::utests::test_sphere_2());
        CHECK(wcl::utests::test_sphere_capsule_1());
        CHECK(wcl::utests::test_sphere_capsule_2());
        CHECK(wcl::utests::test_capsule_1());
        CHECK(wcl::utests::test_capsule_2());
    }
}
