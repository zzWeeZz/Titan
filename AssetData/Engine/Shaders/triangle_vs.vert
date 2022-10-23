#version 460

// dvec3 takes 2 slots
layout (location = 0) in vec3 i_Position;
layout (location = 1) in vec3 i_Color;
layout (location = 2) in vec3 i_Normal;
layout (location = 3) in vec2 i_Texcoord;

layout (binding = 0) uniform MvpBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
} mvpObject;

layout (push_constant) uniform pushConstant
{
    mat4 mdlSpace;
}constant;

layout (location = 0) out vec3 u_FragColor;

void main()
{
    mat4 mvp = mvpObject.proj * mvpObject.view * constant.mdlSpace;
    gl_Position = mvp * vec4(i_Position, 1.0);
    u_FragColor = vec3(i_Texcoord.x, i_Texcoord.y, 0);
}