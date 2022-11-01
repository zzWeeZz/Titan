#version 460

layout(location = 0) in vec3 i_FragColor;
layout(location = 1) in vec2 i_TexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

void main()
{
    vec4 col = texture(texSampler, i_TexCoord);
    col = pow(col, vec4(1.0 / 2.2));
    outColor = col;

}