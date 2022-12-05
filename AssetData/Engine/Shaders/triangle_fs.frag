#version 460

layout (location = 0) in vec3 i_FragColor;
layout (location = 1) in vec3 i_Normal;
layout (location = 2) in vec2 i_TexCoord;
layout (location = 3) in vec3 i_FragNormal;
layout (location = 0) out vec4 o_Color;

layout (binding = 1) uniform sampler2D u_TexSampler;

layout (binding = 2) uniform LightBufferObject
{
    vec4 Direction;
    vec4 Color;
} u_LightObject;


vec3 CalcLight()
{
    float lightIntensity = dot(normalize(i_FragNormal), u_LightObject.Direction.xyz);
    vec3 color = u_LightObject.Color.xyz * u_LightObject.Color.w;
    lightIntensity = max(lightIntensity, 0);
    return color * vec3(lightIntensity);
}

const float AmbientLight = 0.4;

void main()
{
    vec4 col = texture(u_TexSampler, i_TexCoord);
    vec3 lightCol = CalcLight();
    col = col * vec4(lightCol + vec3(AmbientLight), 1.f);
    o_Color = pow(col, vec4(1.0 / 2.2));

}