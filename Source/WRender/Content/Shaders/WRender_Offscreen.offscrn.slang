// Camera stuffs
struct Camera {
  float4x4 proj;
  float4x4 view;
  float px;
  float py;
  float pz;
  float pitch;
  float yaw;
  float roll;
  float sx;
  float sy;
  float sz;
  float angle_of_view;
  float near_clipping;
  float far_clipping;
};

// Lighting Types
// --------------

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

// WLightingUBO
// ------------

struct LightingUBO {
    PointLight point_lights[64];
    DirectionalLight directional_lights[16];
    AmbientLight ambient_light;

    uint point_lights_count;
    uint directional_lights_count;
};

// Global bindings
// ---------------

[[vk::binding(0, 0)]] // binding, set
ConstantBuffer<Camera> camera;

[[vk::binding(1,0)]]
ConstantBuffer<LightingUBO> lighting;


// --

[[vk::binding(0,1)]]            // binding 0, set 1
uniform Sampler2D albedo;

[[vk::binding(1,1)]]            // binding 1, set 1
uniform Sampler2D normal;

[[vk::binding(2,1)]]            // binfing 2, set 1
uniform Sampler2D ws_position;

// TODO input render textures
//  rm (roughness/metallic)
//  emission
//  extras 1 and 2

[[vk::binding(3,1)]]
uniform Sampler2D depth;

struct VSInput {
    float2 inPosition;
    float2 inTexCoord;
};

struct VSOutput
{
    float4 pos : SV_Position;
    float2 fragTexCoord;
};

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

    float3 color = albedo.Sample(uv).xyz * 0.5;

    // float3 color=float3(0.0, 0.0, 0.0);
    float3 wpos = ws_position.Sample(uv).xyz;
    float3 nrm = normal.Sample(uv).xyz;
    float3 alb = albedo.Sample(uv).xyz;
    
    for(uint i=0; i < lighting.point_lights_count; i++) {
        float3 vdist = wpos - lighting.point_lights[i].position.xyz;
        float dist = length(vdist);

        float weight = 1.0 - (clamp(dist / lighting.point_lights[i].radius, 0, 1));

        color += alb * weight; 
    }


    // uint i = 0;
    // float3 vdist = wpos - lighting.point_lights[i].position.xyz;
    // float dist = length(vdist);

    // float weight = 1.0 - (clamp(dist / lighting.point_lights[i].radius, 0, 1));

    // color += alb * weight; 

    

    return float4(color, 1.f);


    // if (1 == lighting.point_lights_count) {
    //     return float4(1,0,0,1);
    // }
    // else {
    //     return float4(0,1,0,1);
    // }


}

