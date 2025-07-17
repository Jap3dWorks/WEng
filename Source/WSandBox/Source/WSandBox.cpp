#include <iostream>
#include <cstdint>
#include <print>
#include <string>
#include <type_traits>

template<typename A, const char na[32], size_t N=0>
struct SA{
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

    constexpr const char* n = "NONE";

    SA<size_t, _other_> a;
    SA<size_t, _other_2_> b;

    static_assert(std::is_same_v<SA<size_t, _other_>, SA<size_t, _other_>>);

    std::print("*{}*\n", a.Name());

    

    return 0;
}

