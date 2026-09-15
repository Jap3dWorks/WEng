#include "WCollision_tests.hpp"

#include <catch2/catch_test_macros.hpp>


TEST_CASE("WCollision") {
    SECTION("WCollision") {
        CHECK(wcl::utests::test_CheckAABB_1());
        CHECK(wcl::utests::test_CheckAABB_2());
        CHECK(wcl::utests::test_box_1());
        CHECK(wcl::utests::test_box_2());
        CHECK(wcl::utests::test_box_3());
        CHECK(wcl::utests::test_box_4());
        CHECK(wcl::utests::test_box_capsule_1());
        CHECK(wcl::utests::test_box_capsule_2());
        CHECK(wcl::utests::test_box_capsule_3());
        CHECK(wcl::utests::test_box_capsule_4());
        CHECK(wcl::utests::test_box_plane_1());
        CHECK(wcl::utests::test_box_plane_2());
        CHECK(wcl::utests::test_box_triangle_1());
        CHECK(wcl::utests::test_box_triangle_2());
        CHECK(wcl::utests::test_box_mesh_1());
        CHECK(wcl::utests::test_box_mesh_2());
        CHECK(wcl::utests::test_sphere_1());
        CHECK(wcl::utests::test_sphere_2());
        CHECK(wcl::utests::test_sphere_capsule_1());
        CHECK(wcl::utests::test_sphere_capsule_2());
        CHECK(wcl::utests::test_point_capsule_1());
        CHECK(wcl::utests::test_point_capsule_2());
        CHECK(wcl::utests::test_capsule_1());
        CHECK(wcl::utests::test_capsule_2());
    }
}
