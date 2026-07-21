module GBuffer;

public struct ModelUBO {
    public float4x4 model;
    public float4x4 normal_matrix;
};

public struct PBRScalar {
    public float4 albedo;    // alpha 1 enhabled.
    public float4 emission;  // same
    public float4 mr;        // same
};

// TODO vert tangent
public struct VSGBufferInput {
    public float3 position;
    public float2 tex_coord;
    public float4 color;
    public float3 normal;
};

public struct VSGBufferOutput {
    public float4 pos        : SV_POSITION;
    public float2 tex_coord  : TEXCOORD0;
    public float3 normal     : TEXCOORD1;
};

public struct FSGBUFferOutput {
    public float4 albedo      : SV_TARGET0;
    public float4 emission    : SV_TARGET1;
    public float4 normal      : SV_TARGET2;
    public float4 mrAO        : SV_TARGET3;
    public float4 extra01     : SV_TARGET4;
};

[[vk::binding(0, 1)]] // binding, set
public ConstantBuffer<ModelUBO> model_ubo;

