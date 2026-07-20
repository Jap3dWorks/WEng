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


float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}


[shader("fragment")]
float4 fsMain(VSTonemappingOutput vert_out) : SV_TARGET
{
    return hdr_color.Sample(vert_out.fragTexCoord);
    
    float4 texColor = hdr_color.Sample(vert_out.fragTexCoord);
    float3 hdr = texColor.rgb;

    // Apply ACES filmic tonemapping (PBR-appropriate)
    float3 mapped = ACESFilm(hdr);

    // Gamma correction (linear → sRGB)
    float3 gamma = pow(mapped, 1.0f / 2.2f);

    return float4(gamma, texColor.a);
}
