module Offscreen;

[[vk::binding(0,1)]]            // binding 0, set 1
public uniform Sampler2D albedo;

[[vk::binding(1,1)]]
public uniform Sampler2D emission;

[[vk::binding(2,1)]]
public uniform Sampler2D normal;

[[vk::binding(3,1)]]
public uniform Sampler2D ws_position;

[[vk::binding(4,1)]]
public uniform Sampler2D mrAO;

[[vk::binding(5,1)]]
public uniform Sampler2D depth;  // future depth + stencil

[[vk::binding(6,1)]]
public uniform Sampler2D extra01;

public struct VSOffscreenInput {
    public float2 position;
    public float2 tex_coord;
};

public struct VSOffscreenOutput
{
    public float4 position : SV_Position;
    public float2 tex_coord;
};
