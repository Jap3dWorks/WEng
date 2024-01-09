#version 450

layout(binding = 0) uniform SUniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec2 InPosition;
layout(location = 1) in vec3 InColor;

layout(location = 0) out vec3 FragColor;

void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(InPosition, 0.0, 1.0);
    FragColor = InColor;
}
