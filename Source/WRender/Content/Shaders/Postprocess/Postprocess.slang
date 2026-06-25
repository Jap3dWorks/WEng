module Postprocess;

public struct PostprocessParamsUBO {
    public float4 param1;
    public float4 param2;
    public float4 param3;
    public float4 param4;
};

// TODO move to set 1 binding 1
[[vk::binding(0,1)]] // binding 0, set 1
public uniform ConstantBuffer<PostprocessParamsUBO> ppcess_params_ubo;

// TODO move color set to set 1 binding 0
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

