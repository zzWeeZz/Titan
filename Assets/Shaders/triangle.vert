#version 450

void main()
{
	const vec3 Positions[3] = vec3[3](
		vec3( 0.0, 0.0, 0.0 ),
		vec3( 1.0, 0.0, 0.0 ),
		vec3( 0.0, 1.0, 0.0 )
	);

	gl_Position = vec4(Positions[gl_VertexIndex], 1.0 );
}