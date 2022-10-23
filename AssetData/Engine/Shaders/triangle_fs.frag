#version 460

layout(location = 0) in vec3 i_FragColor;
layout(location = 1) in vec2 i_TexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

void main()
{
    outColor = texture(texSampler, i_TexCoord);
}