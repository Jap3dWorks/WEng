import GlobalUBO.GlobalUBO;
import GBuffer.GBuffer;

// Set 0 is reserved for GlobalUBOs
// Set 0 binding 0 contains camera_ubo.
// Set 0 binding 1 contiains lighting_ubo.

// Set 1 is reserved for local UBOs.
// Set 1 binding 0 contains graphic_ubo.

[[vk::binding(1, 1)]]  // binding, set
ConstantBuffer<Sampler2D> albedo;

[[vk::binding(2, 1)]]  // binding, set
ConstantBuffer<Sampler2D> emission;

[[vk::binding(3, 1)]]  // binding, set
ConstantBuffer<Sampler2D> normal;

[[vk::binding(4, 1)]]  // binding, set
ConstantBuffer<Sampler2D> mrAO;

// main functions
// --------------

[shader("vertex")]
VSGBufferOutput vsMain(VSGBufferInput input) {
    VSGBufferOutput output;

    output.normal = normalize(mul(float3x3(graphic_ubo.normal_matrix), input.normal));

    float4 wp = mul(graphic_ubo.model, float4(input.position, 1.0));

    output.pos = mul(camera_ubo.proj, mul(camera_ubo.view, wp));
    output.world_pos = wp.xyz;
    output.tex_coord = input.tex_coord;

    return output;
}

[shader("fragment")]
FSGBUFferOutput fsMain(VSGBufferOutput vert_in) : SV_TARGET {
    FSGBUFferOutput output;

    output.albedo = albedo.Sample(vert_in.tex_coord);

    // output.emission = emission.Sample(vert_in.tex_coord);
    output.emission = float4(0.f);

    output.normal = float4(vert_in.normal, 1.f);

    output.ws_position = float4(vert_in.world_pos, 1.f);

    // output.mrAO = mrAO.Sample(vert_in.tex_coord);
    output.mrAO = float(0.f);

    output.extra01 = float4(0.f);

    return output;
}
