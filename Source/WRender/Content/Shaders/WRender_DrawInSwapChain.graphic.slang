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

uniform Sampler2D texture;

[shader("fragment")]
float4 fsMain(VSOutput vertIn) : SV_TARGET
{
    return float4(0.1, 0.0, 0.0, 1.0);
    // return texture.Sample(vertIn.fragTexCoord);
}



