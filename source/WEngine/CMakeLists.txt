cmake_minimum_required (VERSION 3.26.2)

# find_package(glm REQUIRED)

# include_directories(include ${GLM_INCLUDE_DIRS})
include_directories(include)

add_executable(WEng src/WEngine.cpp)
# target_link_libraries(WEng glm)

