import GlobalUBO.GlobalUBO;
import ShadowMap.ShadowMap;

[shader("vertex")]
VSSMOutput vsMain(VSSMInput input) {
    VSSMOutput output;

    output.pos =
        mul(lighting_ubo.shadow_map_projection,
            mul(lighting_ubo.shadow_map_view,
                mul(model_ubo.model, float4(input.pos, 1.f))));

    return output;
}

[shader("fragment")]
void fsMain(VSSMOutput vert_in) : SV_TARGET {}

