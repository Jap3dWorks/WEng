struct VSSwapchainInput {
    float2 inPosition;
    float2 inTexCoord;
};

struct VSSwapchainOutput
{
    float4 pos : SV_Position;
    float2 fragTexCoord;
};

[[vk::binding(0,0)]]
uniform Sampler2D texture;

[shader("vertex")]
VSSwapchainOutput vsMain(VSSwapchainInput input)
{
    VSSwapchainOutput output;

    output.pos = float4(input.inPosition, 0.f, 1.f);
    output.fragTexCoord = input.inTexCoord;

    return output;
}

[shader("fragment")]
float4 fsMain(VSSwapchainOutput vertIn) : SV_TARGET
{
    return texture.Sample(vertIn.fragTexCoord);
}



