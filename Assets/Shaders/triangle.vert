#version 450

layout (location = 0) out vec4 outColor;

layout (push_constant) uniform constants
{
	mat4 transform;
} PushConstant;

void main()
{
	const vec3 positions[3] = vec3[3](
		vec3(0.4f,0.5f, 0.0f),
		vec3(-0.4f,0.5f, 0.0f),
		vec3(0.f,-0.5f, 0.0f)
	);

	const vec4 colors[3] = vec4[3](
		vec4(0.f,0.f, 1.f, 1),
		vec4(0.f,1.f, 0.f, 1),
		vec4(1.f,0.f, 0.f, 1)
	);
	gl_Position = PushConstant.transform * vec4(positions[gl_VertexIndex], 1.0 );
	outColor = colors[gl_VertexIndex];
}