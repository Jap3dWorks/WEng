#include <cstdint>
#include <format>
#include <print>
#include <string>

#define WLOG(msg, ...) std::print( msg ,##__VA_ARGS__ )
#define WFLOG(msg, ...) std::print("[{}] {}\n", __func__, std::format(msg ,##__VA_ARGS__))

void Funcc() {
    WFLOG("Printing {}", "something");

}

int main(int argc, char* argv[])
{
    Funcc();
    return 0;
}
