#version 460

layout(location = 0) in vec3 i_FragColor;
layout(location = 1) in vec3 i_Normal;
layout(location = 2) in vec2 i_TexCoord;

layout(location = 0) out vec4 o_Color;

layout(binding = 1) uniform sampler2D u_TexSampler;

layout(binding = 2) uniform LightBufferObject
{
    vec4 Direction;
    vec4 Color;
} u_LightObject;


vec3 CalcLight()
{
    float lightIntensity = dot(i_Normal, u_LightObject.Direction.xyz);
    vec3 color = u_LightObject.Color.xyz * u_LightObject.Color.w;
    return color * vec3(lightIntensity);
}


void main()
{
    vec4 col = texture(u_TexSampler, i_TexCoord);
    col = pow(col, vec4(1.0 / 2.2));
    vec3 lightCol = CalcLight();
    o_Color = col * vec4(lightCol, 1.f);

}