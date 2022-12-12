#version 460

#extension GL_KHR_vulkan_glsl : enable

layout (location = 0) out vec4 FragColor;

layout (location = 0) in PerVertexData
{
	vec4 color;
	vec3 fragPosition;
	vec3 normal;
	vec3 tangent;
	vec3 biTangent;
	vec2 texCoord;
} fragIn;

layout (binding = 1, set = 1) uniform sampler2D u_Albedo;

layout (binding = 2, set = 1) uniform _lightData
{
	vec4 direction;
	vec4 color;
} u_LightData;

layout (push_constant) uniform constants
{
	uint DebugMeshlets;
};


void main()
{
	float lightStrength = max(dot(fragIn.normal, u_LightData.direction.xyz), 0.0);

	vec3 lighting = lightStrength * (u_LightData.color.xyz * u_LightData.color.w);

	vec4 color = texture(u_Albedo, fragIn.texCoord);
	if(DebugMeshlets > 0)
	{
		color = fragIn.color;
	}
	color.xyz = (vec3(0.3) + lighting) * color.xyz;

	FragColor = pow(color, vec4(1.0 / 2.2));
}