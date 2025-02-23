#version 450

layout(binding = 1) uniform sampler2D TexSampler;

layout(location = 0) in vec3 FragColor;
layout(location = 1) in vec2 FragTexCoord;

layout(location = 0) out vec4 OutColor;

int main()
{
    OutColor = vec4(FragColor, 1.f);
}

