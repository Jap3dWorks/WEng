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

[[vk::binding(0, 0)]] // binding, set
ConstantBuffer<Camera> camera;

struct SUniformBufferObject {
    float4x4 model;
    float3x3 normal_matrix;
};

[[vk::binding(0, 1)]] // binding, set
ConstantBuffer<SUniformBufferObject> ubo;

struct VSInput {
    float3 position;
    float2 tex_coords;
    float3 color;
    float3 normal;
};

struct VSOutput {
    float4 pos        : SV_POSITION;
    float2 tex_coords : TEXCOORD0;
    float3 normal     : TEXCOORD1;
    float3 world_pos  : TEXCOORD2;
    // float depth       : TEXCOORD3;
};

[shader("vertex")]
VSOutput vsMain(VSInput input) {
    VSOutput output;

    output.normal = normalize(mul(ubo.normal_matrix, input.normal));

    float4 wp = mul(ubo.model, float4(input.position, 1.0));

    output.pos = mul(camera.proj, mul(camera.view, wp));
    output.world_pos = wp.xyz;
    output.tex_coords = input.tex_coords;

    // output.depth = output.pos.z / output.pos.w;

    return output;
}

[[vk::binding(1, 1)]]
ConstantBuffer<Sampler2D> texture;

// Output structure
struct FSOutput {
    float4 albedo       : SV_TARGET0;
    float3 normal      : SV_TARGET1;
    float3 ws_position : SV_target2;
    // rm (roughness metallic)
    // emission
    // float3 rt_extra01  : SV_target3;
    // float3 rt_extra02  : SV_target4;
    // float depth     : SV_TARGET6;

};

[shader("fragment")]
FSOutput fsMain(VSOutput vert_in) : SV_TARGET {
    FSOutput output;

    output.albedo = texture.Sample(vert_in.tex_coords);

    // output.albedo = float4(vert_in.normal, 1.0);

    output.normal = vert_in.normal;
    output.ws_position = vert_in.world_pos;

    return output;
}
