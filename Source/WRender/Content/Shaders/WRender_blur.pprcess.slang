struct VSInput {
    float2 inPosition;
    float2 inTexCoord;
};

struct UBOParams {
    float4 param1;
    float4 param2;
    float4 param3;
    float4 param4;
};

[[vk::binding(0,1)]] // binding 0, set 1
uniform ConstantBuffer<UBOParams> ubo_params;

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

[[vk::binding(0,0)]]  // binding 0 , set 0
uniform Sampler2D prev_render;
// SamplerState

[shader("fragment")]
float4 fsMain(VSOutput vertIn) : SV_TARGET
{
    float width, height;

    prev_render.GetDimensions(width, height);

    float aspect = width / height;

    float pxlw = 1.f / width;
    float pxlh = 1.f / height;

    const int range = 1;
    const int samples_count = (range * 2 + 1) * (range * 2 + 1);
    
    float3 samples[samples_count]={0};

    int idx=0;
    for(int i=-range ; i <= range; i++) {
        for (int j=-range; j<=range; j++) {
            float2 coord = float2(pxlw * i, pxlh * j);
            samples[idx] = prev_render.Sample(vertIn.fragTexCoord + coord).xyz;
            idx++;
        }
    }

    float3 color = float3(0.0, 0.0, 0.0);
    for(int i=0; i<samples_count; i++) {
        color+=samples[i];
    }

    color /= samples_count;

    return float4(color, 1.f);
}



