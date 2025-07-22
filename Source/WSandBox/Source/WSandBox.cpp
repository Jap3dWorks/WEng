#include <iostream>
#include <cstdint>
#include <print>
#include <string>
#include <type_traits>
#include <vector>
#include <memory>

struct B {
    virtual ~B() {
        std::print("B Destructor!\n");
    }
};

struct A : public B {
    ~A() override {
        std::print("A Destructor!\n");
    }
};

std::unique_ptr<B> Create() {
    return std::unique_ptr<B>(new A);
}

int main(int argc, char* argv[])
{
    std::print("Init Main\n");
    std::unique_ptr<B> b = Create();
}

