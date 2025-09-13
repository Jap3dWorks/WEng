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

using WShaderEntryPointStr = char[16];
using WShaderEntryPointList = std::array<WShaderEntryPointStr, 8>;

struct TestStruct {
    WShaderEntryPointList entry_points{"main"};
};

int main(int argc, char* argv[])
{

    TestStruct ts;

    for (auto & ep : ts.entry_points) {
        std::print("{}\n", ep);
    }

    return 0;

}

