#include <cstdint>
#include <vector>

struct EngData;

struct A {
public:

    A() {}

    uint32_t val() {
        return a;
    }

private:

    uint32_t a;
};

struct EngData {};

int main(int argc, char* argv[])
{

    std::vector<A> v;

    v.reserve(15);

    EngData b;

    for (uint32_t i=0; i<15; i++) {
        v.push_back(A());
    }

	return 0;

}
