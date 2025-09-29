#version 450

layout(std140, set=0, binding=0) uniform Camera {

  mat4 proj;
  mat4 view;
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

} camera;

layout(set=1, binding=0) uniform SUniformBufferObject {
    mat4 model;
} ubo;

layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec2 InTexCoord;
layout(location = 2) in vec3 InColor;
layout(location = 3) in vec3 InNormal;

layout(location = 0) out vec3 FragColor;
layout(location = 1) out vec2 FragTexCoord;
layout(location = 2) out vec3 FragNormal;

void main()
{
    gl_Position = camera.proj * camera.view * ubo.model * vec4(InPosition, 1.0);

    FragColor = vec3(1.f, 1.f, 1.f);
    FragTexCoord = InTexCoord;
    FragNormal = InNormal;
}

