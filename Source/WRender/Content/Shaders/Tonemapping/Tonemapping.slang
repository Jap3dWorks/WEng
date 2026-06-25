module Tonemapping;

public struct VSTonemappingInput {
    public float2 inPosition;
    public float2 inTexCoord;
};

public struct VSTonemappingOutput
{
    public float4 pos : SV_Position;
    public float2 fragTexCoord;
};

[[vk::binding(0, 0)]] // binding 0, set 0
public uniform Sampler2D hdr_color;
