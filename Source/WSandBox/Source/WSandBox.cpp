#include <cstddef>
#include <type_traits>
#include <unordered_map>
#include <print>
#include <memory>
#include <vector>
#include <concepts>
#include <functional>

void MFun() {
    std::print("MFun\n");
}

struct A {
    void operator()(int i) {}

    void operator()(int i, int l){}
};

int main(int argc, char* argv[])
{
    return 0;
}

