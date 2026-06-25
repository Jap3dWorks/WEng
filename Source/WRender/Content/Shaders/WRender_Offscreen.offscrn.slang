import GlobalUBO.GlobalUBO;
import Offscreen.Offscreen;

// Set 0 is reserved for GlobalUBOs
// Set 0 binding 0 contains camera_ubo.
// Set 0 binding 1 contiains lighting_ubo.

// Set 1 is reserved for local Shader descriptors.
// 

// main functions
// --------------

[shader("vertex")]
VSOffscreenOutput vsMain(VSOffscreenInput input)
{
    VSOffscreenOutput output;

    output.position = float4(input.position, 0.f, 1.f);
    output.tex_coord = input.tex_coord;

    return output;
}

[shader("fragment")]
float4 fsMain(VSOffscreenOutput vertexOutput) : SV_TARGET
{
    float2 uv = vertexOutput.tex_coord;

    float3 wpos = ws_position.Sample(uv).xyz;
    float3 nrm = normal.Sample(uv).xyz;
    float3 alb = albedo.Sample(uv).xyz;

    float3 color = alb *
        lighting_ubo.ambient_light.color.xyz *
        lighting_ubo.ambient_light.intensity;
    
    for(uint i=0; i < lighting_ubo.point_lights_count; i++) {
        float3 vdist = lighting_ubo.point_lights[i].position.xyz - wpos;
        float dist = length(vdist);

        float weight = 1.0 - (clamp(dist / lighting_ubo.point_lights[i].radius, 0, 1));
        float n = clamp(dot(normalize(vdist), nrm), 0, 1);
        
        color += alb * lighting_ubo.point_lights[i].intensity * n * weight; 
    }

    return float4(color, 1.f);

}

