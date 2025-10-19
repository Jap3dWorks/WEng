// Camera stuffs

struct VSInput {
    float3 position;
    float2 tex_coords;
    float3 normal;
};

struct VSOutput {
    float4 pos : SV_POSITION;
    float2 tex_coords;
    float3 normal;
    float3 world_pos;
};

[shader("vertex")]
VSOutput vsMain(VSInput input) {
    VSOutput output;

    output.pos = float4(input.position, 1.f);
    output.tex_coords = input.tex_coords;

    return output;
}

uniform Sampler2D texture;

struct FSOutput {
    float4 color : SV_TARGET0;
    float depth : SV_TARGET1;
    float3 normal : SV_TARGET2;
    float3 worldPos : SV_target3;
};

[shader("fragment")]
FSOutput fsMain(VSOutput vert_in) : SV_TARGET {
    FSOutput output;

    output.color = texture.Sample(vert_in.tex_coords);
    output.normal=vert_in.normal;
    output.worldPos=vert_in.world_pos;

    return output;

}
