#include <cstddef>
#include <type_traits>
#include <unordered_map>
#include <print>
#include <memory>
#include <vector>
#include <cstdint>
#include <concepts>
#include <functional>
#include <regex>
#include <tuple>

#include <string>

std::tuple<int, float, std::string> getThreeValues() {
    return std::make_tuple(42, 3.14f, "hello");
}


int main(int argc, char* argv[])
{

    auto r = getThreeValues();

    int v = std::get<0>(r);

    // Usage
    auto [number, pi, greeting] = getThreeValues();

}

