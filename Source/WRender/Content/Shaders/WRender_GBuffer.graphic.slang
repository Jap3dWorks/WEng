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
};

[shader("vertex")]
VSOutput vsMain(VSInput input) {
    VSOutput output;

    output.normal = normalize(mul(ubo.normal_matrix, input.normal));

    float4 wp = mul(ubo.model, float4(input.position, 1.0));

    output.pos = mul(camera.proj, mul(camera.view, wp));
    output.world_pos = wp.xyz;
    output.tex_coords = input.tex_coords;

    return output;
}

[[vk::binding(1, 1)]]
ConstantBuffer<Sampler2D> texture;

// Output structure
struct FSOutput {
    float4 albedo      : SV_TARGET0;
    float4 normal      : SV_TARGET1;
    float4 ws_position : SV_TARGET2;
    float4 mrAO        : SV_TARGET3;
    float4 emission    : SV_TARGET4;
    float4 extra01     : SV_TARGET5;
};

[shader("fragment")]
FSOutput fsMain(VSOutput vert_in) : SV_TARGET {
    FSOutput output;

    output.albedo = texture.Sample(vert_in.tex_coords);

    output.normal = float4(vert_in.normal, 1.f);

    output.ws_position = float4(vert_in.world_pos, 1.f);

    output.mrAO = float4(0.f);

    output.emission = float4(0.f);

    output.extra01 = float4(0.f);

    return output;
}
