#include <cstddef>
#include <type_traits>
#include <unordered_map>
#include <print>
#include <memory>
#include <vector>
#include <concepts>
#include <functional>

#include "WCore/TFunction.hpp"

void MFun() {
    std::print("MFun\n");
}

struct A {
    void operator()(int i) {}

    void operator()(int i, int l){}
};

int main(int argc, char* argv[])
{
    TFunction<void()> fn(&MFun); 

    fn();

    // TFunPtr<void()> fnp (&MFun);

    // std::function<void()> fn2(MFun);

    // fn2();

    // static_cast<void(A::*)(int,int)>(&A::operator());

    TFnLmbd<64, int()> flm( [](){ std::print("I'm a lambda!\n"); return 2; } );

    flm();

    TFnLmbd<8, void()> fpt(&MFun);

    fpt();
    // void(*ptr)() = MFun;

    return 0;

}

