#version 450

layout(binding = 0) uniform SUniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec2 InTexCoord;
layout(location = 2) in vec3 InColor;

layout(location = 0) out vec3 FragColor;
layout(location = 1) out vec2 FragTexCoord;

void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(InPosition, 1.0);

    FragColor = vec3(1.f, 1.f, 1.f);
    FragTexCoord = InTexCoord;
}

