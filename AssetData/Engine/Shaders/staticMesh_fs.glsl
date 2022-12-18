#version 450

#extension GL_KHR_vulkan_glsl : enable

layout (location = 0) out vec4 FragColor;

layout (location = 1) in PerVertexData
{
	vec4 color;
	vec3 fragPosition;
	vec3 normal;
	vec3 tangent;
	vec3 biTangent;
	vec2 texCoord;
} fragIn;

layout (binding = 5, set = 0) uniform sampler2D u_Albedo;

layout (binding = 6, set = 0) uniform _lightData
{
	vec4 direction;
	vec4 color;
} u_LightData;

layout (push_constant) uniform constants
{
    uint meshletCount;
	uint vertexCount;
	uint indexCount;
	uint meshletOffset;
	uint vertexOffset;
	uint triangleOffset;
	uint vertexIndexOffset;
	uint renderDebugState;
	uint textureId;
};

void main()
{
	float lightStrength = max(dot(fragIn.normal, u_LightData.direction.xyz), 0.0);

	vec3 lighting = lightStrength * (u_LightData.color.xyz * u_LightData.color.w);

	vec4 color = texture(u_Albedo, fragIn.texCoord);
	if(renderDebugState > 0)
	{
		color = fragIn.color;
		FragColor = pow(color, vec4(1.0 / 2.2));
		return;
	}
	color.xyz = (vec3(0.3) + lighting) * color.xyz;

	FragColor = pow(color, vec4(1.0 / 2.2));
}