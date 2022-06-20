#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

layout (location = 0) out vec4 outColor;

layout (push_constant) uniform constants
{
	mat4 transform;
} PushConstant;

void main()
{
	gl_Position = PushConstant.transform * vec4(inPosition, 1.0 );
	outColor = vec4(inTexCoord, 0.f, 1.0f);
}