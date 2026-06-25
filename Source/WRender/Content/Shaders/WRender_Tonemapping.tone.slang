import Tonemapping.Tonemapping;

// Set 0 Binding 0 contains hdr_color Sampler2D. 


// Main functions
// --------------

[shader("vertex")]
VSTonemappingOutput vsMain(VSTonemappingInput input)
{
    VSTonemappingOutput output;

    output.pos = float4(input.inPosition, 0.f, 1.f);
    output.fragTexCoord = input.inTexCoord;

    return output;
}

[shader("fragment")]
float4 fsMain(VSTonemappingOutput vert_out) : SV_TARGET
{

    float3 hdr = hdr_color.Sample(vert_out.fragTexCoord).rgb;

    return float4(hdr, 1.0);

}

