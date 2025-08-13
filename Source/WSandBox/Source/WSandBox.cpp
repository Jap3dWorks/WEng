#include <cstddef>
#include <type_traits>
#include <unordered_map>
#include <print>
#include <memory>
#include <vector>
#include <concepts>
#include <functional>

template<std::uint32_t N>
struct A {
    A()=default;
    ~A()=default;
    A(const A & other) { std::print("Copy constructor A{}\n", N); }
    A(A && other) { std::print("Move constructor A{}\n", N); }
    A & operator=(const A & other) { std::print("Copy assignment A{}\n", N); }
    A & operator=(A && other) { std::print("Move assignment A{}\n", N); }
};

void lmbdfn(const A<0>&, const A<1>&, A<2>, A<3>&&, A<4>*) {
    std::print("The lmbd");
}

template<typename ... Args>
struct F {

    template<typename ... FArgs>
    void MthdA(FArgs && ... args) {
        std::print("--MthdA--\n");
        ManageFn(std::forward<FArgs>(args)...);
    }

    void MthdB(Args ... args) {
        std::print("--MthdB--\n");
        ManageFn(std::forward<Args>(args)...);
    }

    void ManageFn(Args ... args) {
        std::print("--ManageFn--\n");
        lmbdfn(std::forward<Args>(args) ...);
    }

};

int main(int argc, char* argv[])
{

    F<const A<0>&, const A<1>&, A<2>, A<3>&&, A<4>*> fn; 

    A<1> a1;

    A<2> a2;

    A<4> a4;
    A<4>* ptr = &a4;

    fn.MthdA(A<0>(), a1, a2, A<3>(), ptr);

    std::print("--------\n");

    return 0;
}

