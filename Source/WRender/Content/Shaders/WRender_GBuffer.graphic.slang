import GlobalUBO.GlobalUBO;
import GBuffer.GBuffer;

// Set 0 is reserved for GlobalUBOs
// Set 0 binding 0 contains camera_ubo.
// Set 0 binding 1 contiains lighting_ubo.

// Set 1 is reserved for local UBOs.
// Set 1 binding 0 contains graphic_ubo.

[[vk::binding(1, 1)]]  // binding, set
ConstantBuffer<Sampler2D> texture;

// main functions
// --------------

[shader("vertex")]
VSGraphicOutput vsMain(VSGraphicInput input) {
    VSGraphicOutput output;

    output.normal = normalize(mul(float3x3(graphic_ubo.normal_matrix), input.normal));

    float4 wp = mul(graphic_ubo.model, float4(input.position, 1.0));

    output.pos = mul(camera_ubo.proj, mul(camera_ubo.view, wp));
    output.world_pos = wp.xyz;
    output.tex_coord = input.tex_coord;

    return output;
}

[shader("fragment")]
FSOutput fsMain(VSGraphicOutput vert_in) : SV_TARGET {
    FSOutput output;

    output.albedo = texture.Sample(vert_in.tex_coord);

    output.normal = float4(vert_in.normal, 1.f);

    output.ws_position = float4(vert_in.world_pos, 1.f);

    output.mrAO = float4(0.f);

    output.emission = float4(0.f);

    output.extra01 = float4(0.f);

    return output;
}
