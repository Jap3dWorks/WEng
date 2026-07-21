import GlobalUBO.GlobalUBO;
import Lighting.Lighting;

// Set 0 is reserved for GlobalUBOs
// Set 0 binding 0 contains camera_ubo.
// Set 0 binding 1 contiains lighting_ubo.

// Set 1 is reserved for local Shader descriptors.
// 

// main functions
// --------------

[shader("vertex")]
VSLightingOutput vsMain(VSLightingInput input)
{
    VSLightingOutput output;

    output.position = float4(input.position, 0.f, 1.f);
    output.tex_coord = input.tex_coord;

    return output;
}


// PBR functions
float DistributionGGX(float NdotH, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySmith(float NdotV, float NdotL, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float ggx1 = NdotV / (NdotV * (1.0 - k) + k);
    float ggx2 = NdotL / (NdotL * (1.0 - k) + k);

    return ggx1 * ggx2;
}

float3 FresnelSchlick(float cosTheta, float3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 WPos(float depth, float2 uv, float4x4 view, float4x4 inv_view, float4x4 inv_proj) {

    // Convert UV to clip-space coordinates [-1, +1]
    float4 clip_pos = float4(uv * 2.0 - 1.0, depth, 1.0);

    // Clip space -> View space
    float4 view_pos = mul(inv_proj, clip_pos);
    view_pos.xyz /= view_pos.w;

    // View space -> World space
    return mul(inv_view, float4(view_pos.xyz, 1.0)).xyz;
}

static const float PI = 3.14159265359;

[shader("fragment")]
float4 fsMain(VSLightingOutput vertexOutput) : SV_TARGET
{
    float2 uv = vertexOutput.tex_coord;

    // Read G-Buffers
    float dpt = depth.Sample(uv).r;
    float3 wpos = WPos(dpt, uv, camera_ubo.view, camera_ubo.inv_view, camera_ubo.inv_proj);

    float3 nrm = normal.Sample(uv).rgb;
    float3 alb = albedo.Sample(uv).rgb;
    float3 ems = emission.Sample(uv).rgb;

    float2 mr = orm.Sample(uv).rg;
    mr.r = max(mr.r, 0.04);
    mr.g = max(mr.g, 0.04);

    // mr.r = .5;
    // mr.g = .5;

    float ao = orm.Sample(uv).b;

    float3 campos = float3(camera_ubo.px, camera_ubo.py, camera_ubo.pz);
    float3 V = normalize(campos - wpos);

    float3 F0 = lerp(float3(0.04, 0.04, 0.04), alb, mr.x);

    // Acumulator
    float3 Lo = float3(0.0, 0.0, 0.0);

    for(uint i=0; i < lighting_ubo.point_lights_count; i++) {
        float3 Lvec = lighting_ubo.point_lights[i].position.xyz - wpos;
        float dist = length(Lvec);

        // float radius = lighting_ubo.point_lights[i].color_radius.w;
        // Skip if fragment is outside light’s radius
        // if(dist > radius) continue;

        float3 L = Lvec / dist;  // normalized
        float attenuation = 1.0 / (dist * dist + 1e-4);
        float3 radiance = lighting_ubo.point_lights[i].color_radius.rgb * attenuation;

        float3 H = normalize(V + L);
        float NdotL = max(dot(nrm, L), 0.0);
        float NdotV = max(dot(nrm, V), 0.0);
        float NdotH = max(dot(nrm, H), 0.0);
        float HdotV = max(dot(H, V), 0.0);

        float D = DistributionGGX(NdotH, mr.y);
        float G = GeometrySmith(NdotV, NdotL, mr.y);
        float3 F = FresnelSchlick(HdotV, F0);

        float3 specular = (D * G * F) / (4.0 * NdotV * NdotL + 1e-4);

        float3 kD = (1.0 - F) * (1.0 - mr.x);

        Lo += (kD * alb / PI + specular) * radiance * NdotL;
    }

    float3 color = lighting_ubo.ambient_light.color.rgb * alb * ao + Lo + ems;
    // float3 color = lighting_ubo.ambient_light.color.rgb * alb + Lo + ems;

    // PBR directional
    for (uint i=0; i < lighting_ubo.directional_lights_count; i++) {
        float3 L = normalize(-lighting_ubo.directional_lights[i].direction.rgb);
        float NdotL = max(dot(nrm, L), 0.0);
        float3 H = normalize(V + L);
        float NdotH = max(dot(nrm, H), 0.0);
        float HdotV = max(dot(H, V), 0.0);

        float D = DistributionGGX(NdotH, mr.y);
        float G = GeometrySmith(max(dot(nrm, V), 0.0), NdotL, mr.y);
        float3 F = FresnelSchlick(HdotV, F0);

        float3 specular = (D * G * F) / (4.0 * max(dot(nrm, V), 0.0) * NdotL + 1e-4);
        float3 kD = (1.0 - F) * (1.0 - mr.x);
        float3 radiance = lighting_ubo.directional_lights[i].color.rgb;

        color += (kD * alb / PI + specular) * radiance * NdotL;
    }

    return float4(color, 1.0);
}
