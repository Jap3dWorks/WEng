module GBuffer;

public struct ModelUBO {
    public float4x4 model;
    public float4x4 normal_matrix;
};

public struct PBRScalar {
    public float4 albedo;      // multiplier for the input albedo
    public float4 emission;    // same
    public float4 orm_nscale;  // multipliers: occlusion roughness metallic, normal scale.
};

public struct VSGBufferInput {
    public float3 position;
    public float2 tex_coord;
    public float4 color;
    public float3 normal;
    public float4 tangent;
};

public struct VSGBufferOutput {
    public float4 pos        : SV_POSITION;
    public float2 tex_coord  : TEXCOORD0;
    public float3 normal     : TEXCOORD1;
    public float3 tangent    : TEXCOORD2;
    public float3 bitangent  : TEXCOORD3;
};

public struct FSGBUFferOutput {
    public float4 albedo      : SV_TARGET0;
    public float4 emission    : SV_TARGET1;
    public float4 normal      : SV_TARGET2;
    public float4 orm         : SV_TARGET3;
    public float4 extra01     : SV_TARGET4;
};

[[vk::binding(0, 1)]] // binding, set
public ConstantBuffer<ModelUBO> model_ubo;

