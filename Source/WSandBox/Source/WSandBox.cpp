#include <iostream>
#include <cstdint>
#include <print>
#include <string>
#include <type_traits>
#include <vector>

template<typename A, const char na[32], size_t N=0>
struct SA {
    SA():name_("NONE"){}

    const char * Name() { return name_; }

private:
    const char* name_;
};

constexpr const char * _name_ = "NONE";

constexpr const char* GetName() { return "NONE"; }

constexpr const char _other_[32] = "NONE";

constexpr const char _other_2_[32] = "NONE";

int main(int argc, char* argv[])
{
    

    std::vector<uint32_t> v(5);

    std::vector<uint32_t>* ptr = &v;

    auto t = v.begin()[2];

    for (auto& a : *ptr) {
        
    }

    return 0;
}

