#version 450

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 inUV;

layout (set = 0, binding = 1) uniform sampler2D Albedo;

layout (location = 0) out vec4 color;
void main()
{
	vec3 outColor = texture(Albedo, inUV).xyz;
	color = vec4(outColor, 1);
}