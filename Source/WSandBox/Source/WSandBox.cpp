#include <cstddef>
#include <type_traits>
#include <unordered_map>
#include <print>
#include <memory>
#include <vector>
#include <cstdint>
#include <concepts>
#include <functional>
#include <regex>
#include <tuple>

#include <string>

struct WRPParameterStruct;
struct WRPParameterStruct_Texture;
struct WRPParameterStruct_Ubo;

struct WRPParameterVisitor {
    virtual void Visit(WRPParameterStruct *)=0;
    virtual void Visit(WRPParameterStruct_Texture *)=0;
    virtual void Visit(WRPParameterStruct_Ubo*)=0;
};

struct WRPParameterStruct {
    std::uint8_t binding{0};

    virtual void Visit(WRPParameterVisitor * in_visitor) {
        in_visitor->Visit(this);
    }
};

struct WRPParameterStruct_Texture : public WRPParameterStruct {
    std::size_t texture_id;
    void Visit(WRPParameterVisitor * in_visitor) override {
        in_visitor->Visit(this);
    }
};

struct WRPParameterStruct_Ubo : public WRPParameterStruct {
    virtual void* Data() const=0;
    virtual std::size_t Size() const=0;
    std::size_t offset{0};
    
    void Visit(WRPParameterVisitor * in_visitor) override {
        in_visitor->Visit(this);
    }
};

template<std::size_t N>
struct TRPParameterStruct_Ubo : public WRPParameterStruct_Ubo {
    char data[N];
    void * Data() const override { return data; }
    std::size_t Size() const override { return N; }
};

using WRPParameterList = std::array<WRPParameterStruct, 8>;

struct WRenderPipelineParametersStruct {
    WRPParameterList parameter_list{};
    std::uint8_t parameters_counts;
    
    WRPParameterStruct_Texture texture_params{};
    std::uint8_t texture_assets_count{0};
};


struct TestVisitor : public WRPParameterVisitor {
    void Visit(WRPParameterStruct *) override {
        std::println("WRPParameterStruct");
    }
    void Visit(WRPParameterStruct_Texture *) override {
        std::println("WRPParameterStruct_Texture");
    }
    void Visit(WRPParameterStruct_Ubo*) override {
        std::println("WRPParameterStruct_Ubo");
    }
};


int main(int argc, char* argv[])
{

    WRPParameterStruct_Texture txp{};

    TestVisitor vst{};
    txp.Visit(&vst);

    return 0;

}

