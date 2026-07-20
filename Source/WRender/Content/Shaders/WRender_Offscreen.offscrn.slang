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

static const float PI = 3.14159265359;

[shader("fragment")]
float4 fsMain(VSOffscreenOutput vertexOutput) : SV_TARGET
{
    float2 uv = vertexOutput.tex_coord;

    float3 wpos = ws_position.Sample(uv).xyz;
    float3 nrm = normal.Sample(uv).xyz;
    float3 alb = albedo.Sample(uv).xyz;
    float3 emss = emission.Sample(uv).xyz;
    float2 mr = mrAO.Sample(uv).xy;
    float ao = mrAO.Sample(uv).z;

    float3 campos = float3(camera_ubo.px, camera_ubo.py, camera_ubo.pz);

    float3 V = normalize(campos - wpos);
    float3 R = reflect(-V, nrm);

    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, alb.rgb, mr.x);

    float3 Lo = float3(0.0, 0.0, 0.0);

    float3 color;
    // = alb * lighting_ubo.ambient_light.color.xyz;

    for(uint i=0; i < lighting_ubo.point_lights_count; i++) {
        float3 vdist = lighting_ubo.point_lights[i].position.xyz - wpos;
        float dist = length(vdist);

        float3 L = normalize(lighting_ubo.point_lights[i].position.xyz - wpos);

        float attenuation = 1.0 / (dist * dist);
        float3 radiance = lighting_ubo.point_lights[i].color_radius.xyz * attenuation;

        float3 H = normalize(V + L);

        float NdotL = max(dot(nrm, L), 0.0);
        float NdotV = max(dot(nrm, V), 0.0);
        float NdotH = max(dot(nrm, H), 0.0);
        float HdotV = max(dot(H, V), 0.0);

        // Specular BRDF
        float D = DistributionGGX(NdotH, mr.y);
        float G = GeometrySmith(NdotV, NdotL, mr.y);
        float3 F = FresnelSchlick(HdotV, F0);

        float3 numerator = D * G * F;
        float denominator = 4.0 * NdotV * NdotL + 0.0001;
        float3 specular = numerator / denominator;

        // link:https://learnopengl.com/PBR/Theory[Energy conservation]
        float3 kS = F;
        float3 kD = float3(1.0, 1.0, 1.0) - kS;
        kD *= 1.0 - mr.x;

        // Add to outgoing radiance
        Lo += (kD * alb / PI + specular) * radiance * NdotL;

        // float radius_weight =
        //     1.0 - (clamp(dist / lighting_ubo.point_lights[i].color_radius.w, 0, 1));
    }

    color = lighting_ubo.ambient_light.color.xyz + Lo + emss;

    // PBR directional
    for (uint i=0; i < lighting_ubo.directional_lights_count; i++) {
        float3 L = normalize(-lighting_ubo.directional_lights[i].direction.xyz);
        float NdotL = max(dot(nrm, L), 0.0);
        float NdotV_dir = max(dot(nrm, V), 0.0);
        float3 H = normalize(V + L);
        float NdotH = max(dot(nrm, H), 0.0);
        float HdotV = max(dot(H, V), 0.0);

        float D = DistributionGGX(NdotH, mr.y);
        float G = GeometrySmith(NdotV_dir, NdotL, mr.y);
        float3 F = FresnelSchlick(HdotV, F0);

        float3 numerator = D * G * F;
        float denominator = 4.0 * NdotV_dir * NdotL + 0.0001;
        float3 specular = numerator / denominator;

        float3 kS = F;
        float3 kD = (float3(1.0, 1.0, 1.0) - kS) * (1.0 - mr.x);

        float3 radiance = lighting_ubo.directional_lights[i].color.xyz;
        color += (kD * alb / PI + specular) * radiance * NdotL;
    }

    return float4(color, albedo.Sample(uv).a);

}
