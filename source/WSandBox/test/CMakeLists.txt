if(NOT EMSCRIPTEN)

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

# target_compile_options(
#   WSandBox_test "-fpass-plugin=$<TARGET_FILE:some_library>"
#   )

# add_custom_command(
#   TARGET WSandBox_test
#   PRE_BUILD
#   COMMAND ${CMAKE_COMMAND} -E touch_nocreate ${CMAKE_CURRENT_SOURCE_DIR}/WSandBox_test.cc
#   ) 

# config target compiler and extension

include(GoogleTest)
gtest_discover_tests(WSandBox_test)
else()
  # emscripten here
  
endif()
