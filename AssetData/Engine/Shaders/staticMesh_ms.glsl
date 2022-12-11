#version 460


#extension GL_NV_mesh_shader: require

#define  MAX_VERTEX_COUNT 64
#define  MAX_PRIMITIVE_COUNT 124

#define GROUP_SIZE 1

layout (local_size_x = GROUP_SIZE) in;
layout (triangles, max_vertices = MAX_VERTEX_COUNT, max_primitives = MAX_PRIMITIVE_COUNT) out;


taskNV in Task
 {
  uint      baseID;
  uint  subIDs[GROUP_SIZE];
} IN;

layout (location = 0) out PerVertexData
{
	vec4 color;
	vec3 fragPosition;
	vec3 normal;
	vec3 tangent;
	vec3 biTangent;
	vec2 texCoord;
} v_out[];


layout (binding = 0) uniform MvpBufferObject
{
	mat4 view;
	mat4 proj;
	mat4 mdlSpace;
} u_MvpObject;

struct Vertex
{
	vec4 Position;
	vec4 Color;
	vec4 Normal;
	vec4 Tangent; 
	vec2 TexCoords;
	vec2 padding;
};

layout (std430, binding = 1, set = 0) buffer _vertices
{
  Vertex vertices[];
} u_VertexBuffer;

layout (std430, binding = 2, set = 0) buffer _triangles
{
  uint triangles[];
} u_TriangleBuffer;

struct Meshlet
{
  uint vertexOffset;
  uint triangleOffset;
  uint vertexCount;
  uint triangleCount;
};
 
layout (std430, binding = 3, set = 0) buffer _meshlets
{
  Meshlet meshlets[];
} u_MeshletBuffer;

layout (std430, binding = 4, set = 0) buffer _meshletVertices
{
  uint meshletVertices[];
} u_MeshletVertexBuffer;

const uint g_VertexLoops = (MAX_VERTEX_COUNT +  GROUP_SIZE - 1) / GROUP_SIZE;
const uint g_PrimReadLoops = (3 * MAX_PRIMITIVE_COUNT + GROUP_SIZE * 4 - 1) / (GROUP_SIZE * 4);

vec3 GetRandomColor(
	uint _seed)
{
	uint hash = (_seed ^ 61) ^ (_seed >> 16);
	hash = hash + (hash << 3);
	hash = hash ^ (hash >> 4);
	hash = hash * 0x27d4eb2d;
	hash = hash ^ (hash >> 15);
	return vec3(float(hash & 255),
				float((hash >> 8) & 255),
				float((hash >> 16) & 255)) / 255.0;
}
void main()
{
	const uint meshletIndex = IN.baseID + IN.subIDs[gl_WorkGroupID.x];
	const uint threadId = gl_LocalInvocationID.x;
	Meshlet meshlet = u_MeshletBuffer.meshlets[meshletIndex];

	for(uint loop = 0; loop < g_VertexLoops; ++loop)
	{
		uint v = threadId + loop * GROUP_SIZE;

		v = min(v, meshlet.vertexCount - 1);

		const uint VertexIndex = u_MeshletVertexBuffer.meshletVertices[meshlet.vertexOffset + v];
		Vertex vertex = u_VertexBuffer.vertices[VertexIndex];

		const mat4 mvp = u_MvpObject.proj * u_MvpObject.view * u_MvpObject.mdlSpace;
		vec4 fragPosition = mvp * vertex.Position;
		gl_MeshVerticesNV[v].gl_Position = fragPosition;
		v_out[v].fragPosition = fragPosition.xyz;
		v_out[v].color = vec4(GetRandomColor(meshletIndex),1.f);
		v_out[v].texCoord = vertex.TexCoords;
		v_out[v].normal = mat3(u_MvpObject.mdlSpace) * vertex.Normal.xyz;
	}


	uint primreadBegin = meshlet.triangleOffset;
	uint primreadIndex = meshlet.triangleCount * 3;
	uint primreadMax   = primreadIndex;
	for(uint loop = 0; loop < primreadIndex; ++loop)
	{
		uint p = threadId + loop * GROUP_SIZE;
		p = min(p, primreadMax);
		 uint topology = u_TriangleBuffer.triangles[primreadBegin + loop];

		gl_PrimitiveIndicesNV[p] = topology;
	}

	if(threadId == 0)
	{
		gl_PrimitiveCountNV = meshlet.triangleCount;
	}
}