import GlobalUBO.GlobalUBO;
import GBuffer.GBuffer;

public struct VSSMInput {
    public float3 pos;
};

struct VSSMOutput {
    float4 pos : SV_Position;
};

struct FSSMOutput {
    // float4 value;
};

[shader("vertex")]
VSSMOutput vsMain(VSSMInput input) {
    VSSMOutput output;

    output.pos =
        mul(lighting_ubo.shadow_map_projection,
            mul(lighting_ubo.shadow_map_view,
                mul(model_ubo.model, float4(input.pos, 0.f))));

    return output;
}

[shader("fragment")]
FSSMOutput fsMain(VSSMOutput vert_in) : SV_TARGET {
    FSSMOutput output;
    return output;
}
