struct VSInput {
    float3 position;
    float3 texCoords;
    float3 normal;
};

struct VSOutput {
    float4 pos : SV_Position;
    float2 fragTexCoords;
    float3 normal;
};

[shader("vertex")]
VSOuput vsMain(VSInput input) {
    return {};
}

[shader("fragment")]
float4 fsMain(VSOutput vert_in) : SV_TARGET {
    return {};
}
