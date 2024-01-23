#version 450

layout(location = 0) in vec3 FragColor;
layout(location = 0) out vec4 OutColor;

void main()
{
    vec2 Coord = gl_PointCoord - vec2(0.5);
    OutColor = vec4(FragColor, 0.5 - length(Coord));
}