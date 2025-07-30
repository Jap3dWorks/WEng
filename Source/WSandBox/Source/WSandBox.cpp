#include <unordered_map>
#include <print>
#include <memory>
#include <vector>
#include <concepts>

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

template<typename To, typename From>
concept CConvertibleTo = std::is_convertible_v<From*, To*>;

template<typename T, CConvertibleTo<T> P>
struct TT {};


template<typename T>
inline constexpr RetType KK = ImporterKey<T>::Value();


int main(int argc, char* argv[])
{
    static_assert(KK<B> == ImporterKey<B>::Value());

    std::vector<size_t> v={1,2,3,4,5};

    const std::vector<size_t> & cv = v;

    auto it = cv.begin();

    const std::unique_ptr<char> p = std::make_unique<char>('9');

    // p = std::make_unique<char>('j');

    const size_t & _v = cv[1];

    uint8_t a;
    uint8_t* i = &a;
    void* kk = i;

    
    
}

