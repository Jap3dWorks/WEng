struct VSSMInput {
    float2 pos;
    float2 tex_coord;
};


struct VSSMOutput {
    float4 value : SV_Position;
};

struct FSSMOutput {
    float4 value;
};

[shader("vertex")]
VSSMOutput vsMain(VSSMInput input) {
    VSSMOutput output;
    return output;
    // float3x3 normal_matrix = float3x3(model_ubo.normal_matrix);

    // float4 wp = mul(model_ubo.model, float4(input.position, 1.0));

    // output.pos = mul(camera_ubo.proj, mul(camera_ubo.view, wp));

    // output.tex_coord = input.tex_coord;

    // output.normal = normalize(mul(normal_matrix, input.normal));
    // output.tangent = normalize(mul(normal_matrix, input.tangent.xyz));
    // output.bitangent = input.tangent.w * cross(output.normal, output.tangent);

    // return output;
}

[shader("fragment")]
FSSMOutput fsMain(VSSMOutput vert_in) : SV_TARGET {
    FSSMOutput output;
    return output;

    // output.albedo = float4(pbr_scalars.albedo.rgb, 1.f) * albedo.Sample(vert_in.tex_coord);

    // output.emission = float4(pbr_scalars.emission.rgb, 1.f) * emission.Sample(vert_in.tex_coord);

    // // Compute normal from texture with vertex normal.
    // float3x3 tbn = float3x3(vert_in.tangent, vert_in.bitangent, vert_in.normal);
    // float3 snrm = normalize(normal.Sample(vert_in.tex_coord).rgb * 2.f - 1.f);
    // output.normal = float4(mul(snrm, tbn), 1.f);
    
    // output.orm = float4(pbr_scalars.orm_nscale.rgb * orm.Sample(vert_in.tex_coord).rgb, 1.f);

    // // TODO output texcoords and tangents output

    // output.extra01=float4(vert_in.tangent,1.f);

    // return output;
}
