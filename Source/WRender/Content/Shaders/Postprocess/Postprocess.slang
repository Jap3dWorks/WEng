module Postprocess;

// import ..GlobalUBO.GlobalUBO;

// [[vk::binding(0,2)]]  // binding 2, set 0
// public uniform Sampler2D color;

[[vk::binding(1,2)]]
public uniform Sampler2D albedo;

[[vk::binding(2,2)]]
public uniform Sampler2D normal;

[[vk::binding(3,2)]]
public uniform Sampler2D ws_position;

// TODO input render textures
//  rm (roughness/metallic)
//  emission
//  extras 1 and 2

[[vk::binding(4,2)]]
public uniform Sampler2D depth;

// --

// // TODO move color set to set 2 binding 0
[[vk::binding(0,0)]]  // binding 0, set #
public uniform Sampler2D color;

public struct VSPostprocessInput {
    public float2 inPosition;
    public float2 inTexCoord;
};

public struct VSPostprocessOutput
{
    public float4 pos : SV_Position;
    public float2 fragTexCoord;
};

