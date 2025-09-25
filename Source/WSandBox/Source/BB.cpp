#include "BB.hpp"
#include "AA.hpp"

#include "WCore/TClassKey.hpp"
#include <print>

void BB::mthdB() {
    AA aa{};
    aa.mthdA();
    std::print("AA static address: {}\n", TClassKey_v<AA>);
    std::print("BB static address: {}\n", TClassKey_v<BB>);
}


