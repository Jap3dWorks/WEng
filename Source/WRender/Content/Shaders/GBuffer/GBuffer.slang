module GBuffer;

public struct GraphicUBO {
    public float4x4 model;
    public float4x4 normal_matrix;
};

public struct VSGraphicInput {
    public float3 position;
    public float2 tex_coord;
    public float3 color;
    public float3 normal;
};

public struct VSGraphicOutput {
    public float4 pos        : SV_POSITION;
    public float2 tex_coord  : TEXCOORD0;
    public float3 normal     : TEXCOORD1;
    public float3 world_pos  : TEXCOORD2;
};

public struct FSOutput {
    public float4 albedo      : SV_TARGET0;
    public float4 normal      : SV_TARGET1;
    public float4 ws_position : SV_TARGET2;
    public float4 mrAO        : SV_TARGET3;
    public float4 emission    : SV_TARGET4;
    public float4 extra01     : SV_TARGET5;
};

[[vk::binding(0, 1)]] // binding, set
public ConstantBuffer<GraphicUBO> graphic_ubo;

