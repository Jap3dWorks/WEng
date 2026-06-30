module Postprocess;

import "../GlobalUBO/GlobalUBO.slang";

[[vk::binding(0,2)]]  // binding 0, set 2
public uniform Sampler2D previous;

[[vk::binding(1,2)]]  // binding 1, set 2
public uniform Sampler2D color;

[[vk::binding(2,2)]]
public uniform Sampler2D albedo;

[[vk::binding(3,2)]]
public uniform Sampler2D normal;

[[vk::binding(4,2)]]
public uniform Sampler2D ws_position;

// TODO input render textures
//  rm (roughness/metallic)
//  emission
//  extras 1 and 2

[[vk::binding(5,2)]]
public uniform Sampler2D depth;

// --

public struct VSPostprocessInput {
    public float2 position;
    public float2 tex_coord;
};

public struct VSPostprocessOutput
{
    public float4 position : SV_Position;
    public float2 tex_coord;
};

