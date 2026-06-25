module Offscreen;

[[vk::binding(0,1)]]            // binding 0, set 1
public uniform Sampler2D albedo;

[[vk::binding(1,1)]]            // binding 1, set 1
public uniform Sampler2D normal;

[[vk::binding(2,1)]]            // binfing 2, set 1
public uniform Sampler2D ws_position;

// TODO input render textures
//  rm (roughness/metallic)
//  emission
//  extras 1 and 2

[[vk::binding(3,1)]]
public uniform Sampler2D depth;


public struct VSOffscreenInput {
    public float2 position;
    public float2 tex_coord;
};

public struct VSOffscreenOutput
{
    public float4 position : SV_Position;
    public float2 tex_coord;
};
