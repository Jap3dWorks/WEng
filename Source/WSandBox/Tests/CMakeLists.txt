find_package(googletest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

add_executable(
  WSandBox_test
  WSandBox_test.cc
)
target_link_libraries(
  WSandBox_test
  GTest::gtest_main
)


include(GoogleTest)
gtest_discover_tests(WSandBox_test)
