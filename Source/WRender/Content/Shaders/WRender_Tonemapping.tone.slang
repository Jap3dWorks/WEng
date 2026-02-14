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

[[vk::binding(0, 0)]] // binding 0, set 0
uniform Sampler2D hdr_color;

[shader("fragment")]
float4 fsMain(VSOutput vert_out) : SV_TARGET
{
    return float4(hdr_color.Sample(vert_out.fragTexCoord).rgb, 1.f);
    
    /*
    float3 hdr = hdr_color.Sample(vert_out.fragTexCoord).rgb;

    // // exposure
    // // hdr *= 1.f;

    // 2.ACES tonemapping
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    float3 ldr = (hdr * (a * hdr + b)) / (hdr * (c * hdr + d) + e);
    ldr = clamp(ldr, 0.0, 1.0);

    ldr = pow(ldr, 1.0/2.2);

    return float4(ldr, 1.0);
    */
}

