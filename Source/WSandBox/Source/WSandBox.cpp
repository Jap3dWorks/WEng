#include <unordered_map>
#include <print>
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

using RetType = const void*;

template<typename T>
struct ImporterKey {

    static inline constexpr const void * _v{nullptr};

    static inline constexpr const void * Value() {
        return &_v;
    }

    // static inline constexpr const void * v=Value();
};

template<typename T>
inline constexpr RetType KK = ImporterKey<T>::Value();


int main(int argc, char* argv[])
{
    std::unordered_map<const void*, std::string> v;

    static_assert(KK<B> == ImporterKey<B>::Value());

    v[KK<B>] = "B value!";
    v[ImporterKey<A>::Value()] = "A Value!";

    std::print("B val: {}\n", v[ImporterKey<B>::Value()]);
    std::print("A val: {}\n", v[ImporterKey<A>::Value()]);

}

