module ShadowMap;

import "../Model/Model.slang";

public struct VSSMInput {
    public float3 pos;
};

public struct VSSMOutput {
    public float4 pos : SV_POSITION;
};

// binding 0, set 1
[[vk::binding(0, 1)]]
public ConstantBuffer<ModelUBO> model_ubo;

