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
        lighting_ubo.ambient_light.color.xyz;

    for(uint i=0; i < lighting_ubo.point_lights_count; i++) {
        float3 vdist = lighting_ubo.point_lights[i].position.xyz - wpos;
        float dist = length(vdist);

        float radius_weight =
            1.0 - (clamp(dist / lighting_ubo.point_lights[i].color_radius.w, 0, 1));

        float n = clamp(dot(normalize(vdist), nrm), 0, 1);
        
        color += alb *
            lighting_ubo.point_lights[i].color_radius.xyz *
            n *
            radius_weight; 
    }

    for (uint i=0; i < lighting_ubo.directional_lights_count; i++) {
        float3 ldir = lighting_ubo.directional_lights[i].direction.xyz;
        
        float lnrm = clamp(
            dot(-nrm,
                lighting_ubo.directional_lights[i].direction.xyz),
            0,
            1);

        color += alb * lnrm * lighting_ubo.directional_lights[i].color.xyz;

    }

    return float4(color, 1.f);

}

