import GlobalUBO.GlobalUBO;
import GBuffer.GBuffer;

// Set 0 is reserved for GlobalUBOs
// Set 0 binding 0 contains camera_ubo.
// Set 0 binding 1 contiains lighting_ubo.

// Set 1 is reserved for local UBOs.
// Set 1 binding 0 contains model_ubo.

[[vk::binding(1,1)]]
ConstantBuffer<PBRScalar> pbr_scalars;

[[vk::binding(2, 1)]]  // binding, set
ConstantBuffer<Sampler2D> albedo;

[[vk::binding(3, 1)]]  // binding, set
ConstantBuffer<Sampler2D> emission;

[[vk::binding(4, 1)]]  // binding, set
ConstantBuffer<Sampler2D> normal;

[[vk::binding(5, 1)]]  // binding, set
ConstantBuffer<Sampler2D> orm;

// main functions
// --------------

[shader("vertex")]
VSGBufferOutput vsMain(VSGBufferInput input) {
    VSGBufferOutput output;

    float3x3 normal_matrix = float3x3(model_ubo.normal_matrix);

    output.normal = normalize(mul(normal_matrix, input.normal));

    float4 wp = mul(model_ubo.model, float4(input.position, 1.0));

    output.pos = mul(camera_ubo.proj, mul(camera_ubo.view, wp));

    output.tex_coord = input.tex_coord;

    return output;
}

[shader("fragment")]
FSGBUFferOutput fsMain(VSGBufferOutput vert_in) : SV_TARGET {
    FSGBUFferOutput output;

    output.albedo = float4(pbr_scalars.albedo.rgb, 1.f) * albedo.Sample(vert_in.tex_coord);

    output.emission = float4(pbr_scalars.emission.rgb, 1.f) * emission.Sample(vert_in.tex_coord);

    // Compute normal from texture with vertex normal.
    // tangent is required to perform this operation
    
    output.normal = float4(vert_in.normal, 1.f);
    
    // temp solution bacause of black AO
    float4 ormsample = orm.Sample(vert_in.tex_coord);
    output.orm = float4(ormsample.r, pbr_scalars.rm.rg * ormsample.gb, 1.f);
    // output.orm = float4(1.f, pbr_scalars.rm.rg, 1.f);
    // output.orm = float4(1.f, 0.5, 0.5, 1.f);


    // TODO output texcoords and tangents output

    output.extra01 = float4(0.f);

    return output;
}
