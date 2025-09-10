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

#include <string>

int main(int argc, char* argv[])
{

    std::string str="/mnt/data/Devs/WEng/WEng/Install/Content/Shaders/somthing.vert.graphics.glsl";

    std::regex extension_pattern("(.*)(\\.[^\\.]+)$");
    std::smatch extension_match;

    if (!std::regex_search(
            str,
            extension_match,
            extension_pattern
            ))
    {
        return {};
    }

    for(auto& grp : extension_match) {
        std::print("- grp : {}\n", grp.str());
    }

    return 0;
}

