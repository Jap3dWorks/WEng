#include <cstddef>
#include <type_traits>
#include <unordered_map>
#include <print>
#include <memory>
#include <vector>
#include <concepts>
#include <functional>

template<typename T>
struct _WId{
public:
    using IdType=T;
};

using WIdInt = _WId<int>;

template<typename T>
concept CIsWId = std::is_same_v<T, _WId<typename T::IdType>>;

template<CIsWId T>
struct TTest{};

int main(int argc, char* argv[])
{
    TTest<_WId<int>> val;

    TTest<WIdInt> val2;

    return 0;
    // TTest<int> val;
}

