#version 450

layout(location = 0) in vec2 InPosition;
layout(location = 1) in vec4 InColor;

layout(location = 0) out vec3 FragColor;

void main()
{
    gl_PointSize = 14.f;
    gl_Position = vec4(InPosition, 0.f, 1.f);
    FragColor = InColor.rgb;
}