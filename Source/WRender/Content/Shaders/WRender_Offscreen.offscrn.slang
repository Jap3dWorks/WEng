struct VSInput {
    float2 inPosition;
    float2 inTexCoord;
};

struct VSOutput
{
    float4 pos : SV_Position;
    float2 fragTexCoord;
};

// TODO lighting data
// ------------------

struct PointLight {
    float4 color;
    float4 position;
    float intensity;
    float radius;
};

struct DirectionalLight {
    float4 color;
    float4 direction;
    float intensity;
};

struct AmbientLight {
    float4 color;
    float intensity;
};

// TODO lighting UBOs
// ------------------

struct LightingUBO {
    PointLight point_lights[128];
    DirectionalLight directional_lights[32];
    AmbientLight ambient_light;

    uint point_lights_count;
    uint directional_lights_count;
};

// bindings
// --------

[[vk::binding(0,0)]]            // binding 0, set 0
uniform Sampler2D albedo;

[[vk::binding(1,0)]]            // binding 1, set 0
uniform Sampler2D normal;

[[vk::binding(2,0)]]            // binfing 2, set 0
uniform Sampler2D ws_position;

// TODO input render textures
//  rm (roughness/metallic)
//  emission
//  extras 1 and 2

[[vk::binding(3,0)]]
uniform Sampler2D depth;

// --
// [[vk::binding(0,0)]] // camera
// ConstantBuffer<LightingUBO> lighting;

// [[vk::binding(1,0)]] // lighting
// --

// main functions
// --------------

[shader("vertex")]
VSOutput vsMain(VSInput input)
{
    VSOutput output;

    output.pos = float4(input.inPosition, 0.f, 1.f);
    output.fragTexCoord = input.inTexCoord;

    return output;
}

[shader("fragment")]
float4 fsMain(VSOutput vertexOutput) : SV_TARGET
{
    float2 uv = vertexOutput.fragTexCoord;

    // float3 color=float3(0.0, 0.0, 0.0);
    // float3 wpos = ws_position.Sample(uv).xyz;
    // float3 nrm = normal.Sample(uv).xyz;
    // float3 alb = albedo.Sample(uv).xyz;
    
    // for(uint i=0; i < lighting.point_lights_count; i++) {
    //     float3 vdist = wpos - lighting.point_lights[i].position.xyz;
    //     float dist = length(vdist);

    //     float weight = 1.0 - (clamp(dist / lighting.point_lights[i].radius, 0, 1));

    //     color += alb * weight; 
    // }

    // return float4(color, 1.f);

    return albedo.Sample(uv);

}

