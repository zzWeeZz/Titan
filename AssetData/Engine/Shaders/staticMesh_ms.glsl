#version 450


#extension GL_NV_mesh_shader: require
#extension GL_EXT_shader_explicit_arithmetic_types: require

#define  MAX_VERTEX_COUNT 64
#define  MAX_PRIMITIVE_COUNT 124

#define GROUP_SIZE 1

layout (local_size_x = GROUP_SIZE) in;
layout (triangles, max_vertices = MAX_VERTEX_COUNT, max_primitives = MAX_PRIMITIVE_COUNT) out;


taskNV in Task
 {
  uint  baseID;
  uint  subIDs[GROUP_SIZE];
} IN;

layout (location = 1) out PerVertexData
{
	vec4 color;
	vec3 fragPosition;
	vec3 normal;
	vec3 tangent;
	vec3 biTangent;
	vec2 texCoord;
} v_out[];

struct Vertex
{
	vec3 Position;
	uint8_t Normal[2];
	uint8_t Tangent[2]; 
	vec4 Color;
	vec2 TexCoords;
	vec2 padding;
};

struct Meshlet
{
  uint vertexOffset;
  uint triangleOffset;
  uint vertexCount;
  uint triangleCount;
  uint meshId;
  uint padd[3];
};

struct Mesh
{
	uint meshletOffset;
	uint vertexOffset;
	uint triangleOffset;
	uint vertexIndexOffset;
	mat4 transform;
};

layout (binding = 0, set = 0) uniform MvpBufferObject
{
	mat4 view;
	mat4 proj;
	mat4 mdlSpace;
} u_MvpObject;

layout (binding = 1, set = 0) readonly buffer _meshes
{
	Mesh meshes[];
};

layout (std430, binding = 1, set = 1) readonly buffer _meshlets
{
  Meshlet meshlets[];
} u_MeshletBuffer;

layout (std430, binding = 2, set = 1) readonly buffer _vertices
{
  Vertex vertices[];
} u_VertexBuffer;

layout (std430, binding = 3, set = 1) readonly buffer _triangles
{
  uint triangles[];
} u_TriangleBuffer;

layout (std430, binding = 4, set = 1) readonly buffer _meshletVertices
{
  uint meshletVertices[];
} u_MeshletVertexBuffer;

const uint g_VertexLoops = (MAX_VERTEX_COUNT +  GROUP_SIZE - 1) / GROUP_SIZE;
const uint g_PrimReadLoops = (3 * MAX_PRIMITIVE_COUNT + GROUP_SIZE * 4 - 1) / (GROUP_SIZE * 4);

layout (push_constant) uniform constants
{
 	uint meshletCount;
	uint vertexCount;
	uint indexCount;
	uint renderDebugState;
};

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

vec3 DecodeOctahedronVectors(vec2 f)
{
	// https://twitter.com/Stubbesaurus/status/937994790553227264

	f = f * 2.0f - 1.0f;

	vec3 n = vec3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));
	float t = clamp(-n.z, 0.f, 1.f);
	if (n.x >= 0.0f || n.y >= 0.0f) // maybe should be && instead of ||
	{
		n.x += -t;
		n.y += -t;
	}
	else
	{
		n.x += t;
		n.y += t;
	}

	return normalize(n);
}

vec3 DecodeNormalEncode(uint8_t data[2])
{
		vec2 quantizeNormal = vec2(data[0] / (pow(2, 8) - 1), data[1] / (pow(2, 8) - 1));
		vec3 normal = DecodeOctahedronVectors(quantizeNormal);
		return normal;
}
void main()
{
	const uint meshletIndex = IN.baseID + IN.subIDs[gl_WorkGroupID.x];
	const uint threadId = gl_LocalInvocationID.x;
	Meshlet meshlet = u_MeshletBuffer.meshlets[meshletIndex];
	Mesh mesh = meshes[meshlet.meshId];
	const uint vertOffset = meshlet.vertexOffset + mesh.vertexIndexOffset;
	for(uint loop = 0; loop < g_VertexLoops; ++loop)
	{
		uint v = threadId + loop * GROUP_SIZE;

		v = min(v, meshlet.vertexCount - 1);
		const uint VertexIndex = u_MeshletVertexBuffer.meshletVertices[vertOffset + v];
		Vertex vertex = u_VertexBuffer.vertices[VertexIndex + mesh.vertexOffset];

		const mat4 mvp = u_MvpObject.proj * u_MvpObject.view * mesh.transform;
		vec4 fragPosition = mvp * vec4(vertex.Position, 1.0f);
		gl_MeshVerticesNV[v].gl_Position = fragPosition;
		v_out[v].fragPosition = fragPosition.xyz;
		v_out[v].color = vec4(GetRandomColor(meshletIndex),1.f);
		v_out[v].texCoord = vertex.TexCoords;
		
		vec3 normal = DecodeNormalEncode(vertex.Normal);
		vec3 tangent = DecodeNormalEncode(vertex.Tangent);
		vec3 biTangent = cross(normal, tangent);
		mat3 TBN = {tangent, biTangent, normal};

		v_out[v].normal = mat3(mesh.transform) * normal;
	}


	uint primreadBegin = meshlet.triangleOffset + mesh.triangleOffset;
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