import GlobalUBO.GlobalUBO;
import GBuffer.GBuffer;

[[vk::binding(0, 1)]] // binding 0, set 1
public ConstantBuffer<ModelUBO> model_ubo;


public struct VSSMInput {
    public float3 pos;
};

struct VSSMOutput {
    float4 pos : SV_POSITION;
};

struct FSSMOutput {};

[shader("vertex")]
VSSMOutput vsMain(VSGBufferInput input) {
    VSSMOutput output;

    output.pos =
        mul(lighting_ubo.shadow_map_projection,
            mul(lighting_ubo.shadow_map_view,
                mul(model_ubo.model, float4(input.position, 1.f))));

    return output;
}

[shader("fragment")]
void fsMain(VSSMOutput vert_in) : SV_TARGET {
    // FSSMOutput output;
    // return output;
}
