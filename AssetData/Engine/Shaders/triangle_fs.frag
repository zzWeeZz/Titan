#version 460

layout(location = 0) in vec3 i_FragColor;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(i_FragColor, 1.0);
}