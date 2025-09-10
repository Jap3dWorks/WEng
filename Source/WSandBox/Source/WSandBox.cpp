#include <cstddef>
#include <type_traits>
#include <unordered_map>
#include <print>
#include <memory>
#include <vector>
#include <cstdint>
#include <concepts>
#include <functional>

#include <string>

int main(int argc, char* argv[])
{

    std::string str="1jdkvuaaa";

    if (str.ends_with("a")) {
        std::print("Very Ok!\n");
    }

    // std::print("float size {}\n", sizeof(float));

    

    return 0;
}

