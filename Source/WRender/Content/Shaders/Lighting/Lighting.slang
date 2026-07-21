module Lighting;

[[vk::binding(0,1)]]            // binding 0, set 1
public uniform Sampler2D albedo;

[[vk::binding(1,1)]]
public uniform Sampler2D emission;

[[vk::binding(2,1)]]
public uniform Sampler2D normal;

[[vk::binding(3,1)]]
public uniform Sampler2D orm;

[[vk::binding(4,1)]]
public uniform Sampler2D depth;  // future depth + stencil

[[vk::binding(5,1)]]
public uniform Sampler2D extra01;

public struct VSLightingInput {
    public float2 position;
    public float2 tex_coord;
};

public struct VSLightingOutput
{
    public float4 position : SV_Position;
    public float2 tex_coord;
};
