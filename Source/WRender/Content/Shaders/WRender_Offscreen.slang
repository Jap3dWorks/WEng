struct VSInput {
    float2 inPosition;
    float2 inTexCoord;
};

struct VSOutput
{
    float4 pos : SV_Position;
    float2 fragTexCoord;
};

[shader("vertex")]
VSOutput vsMain(VSInput input)
{
    VSOutput output;

    output.pos = float4(input.inPosition, 0.f, 1.f);
    output.fragTexCoord = input.inTexCoord;

    return output;
}

[[vk::binding(0,0)]] // binding 0, set 1
uniform Sampler2D albedo;
[[vk::binding(1,0)]] // binding 1, set 0
uniform Sampler2D normal;
[[vk::binding(2,0)]]
uniform Sampler2D ws_position;

// rm (roughness/metallic)
// emission

// extras 1 and 2

[[vk::binding(3,0)]]
uniform Sampler2D depth;

[shader("fragment")]
float4 fsMain(VSOutput vertexOuptut) : SV_TARGET
{
    return albedo.Sample(vertexOuptut.fragTexCoord);
}
