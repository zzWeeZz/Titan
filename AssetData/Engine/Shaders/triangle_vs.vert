#version 460

// dvec3 takes 2 slots
layout (location = 0) in vec3 i_Position;
layout (location = 1) in vec3 i_Color;
layout (location = 2) in vec3 i_Normal;
layout (location = 3) in vec2 i_Texcoord;

layout (binding = 0) uniform MvpBufferObject
{
    mat4 view;
    mat4 proj;
} u_MvpObject;

layout (push_constant) uniform pushConstant
{
    mat4 mdlSpace;
} u_Constant;

layout (location = 0) out vec3 o_FragColor;
layout (location = 1) out vec3 o_Normal;
layout (location = 2) out vec2 o_TexCoord;
layout (location = 3) out vec3 o_FragNormal;

void main()
{
    mat4 mvp = u_MvpObject.proj * u_MvpObject.view * u_Constant.mdlSpace;
    gl_Position = mvp * vec4(i_Position, 1.0);
    o_FragColor = vec3(i_Texcoord.x, i_Texcoord.y, 0);
    o_Normal = i_Normal;
    o_FragNormal = mat3(mvp) * i_Normal;
    o_TexCoord = i_Texcoord;
}