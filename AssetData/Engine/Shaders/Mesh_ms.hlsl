
#ifndef MAX_VERTEX_COUNT 
#define  MAX_VERTEX_COUNT 64
#endif
#ifndef MAX_PRIMITIVE_COUNT 
#define  MAX_PRIMITIVE_COUNT 124
#endif
#define MAX(x, y) (x > y ? x : y)
#define ROUNDUP(x, y) ((x + y - 1) & ~(y - 1))
#define MS_GROUP_SIZE ROUNDUP(MAX(MAX_VERTEX_COUNT, MAX_PRIMITIVE_COUNT), 32)
float3 DecodeOctahedronVectors(in float2 f)
{
    f = f * 2.0 - 1.0;
 
    // https://twitter.com/Stubbesaurus/status/937994790553227264
    float3 n = float3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));
    float t = saturate(-n.z);
    n.xy += n.xy >= 0.0 ? -t : t;
    return normalize(n);
}

float3 DecodeNormalEncode(uint data[2])
{
    const float2 quantizeNormal = float2(data[0] / ((2 * 2 * 2 * 2 * 2 * 2 * 2 * 2) - 1), data[1] / ((2 * 2 * 2 * 2 * 2 * 2 * 2 * 2) - 1));
    const float3 normal = DecodeOctahedronVectors(quantizeNormal);
    return normal;
}

struct Meshlet
{
    uint vertexOffset;
    uint triangleOffset;
    uint vertexCount;
    uint triangleCount;
    uint meshId;
    uint padd[3];
};

struct Vertex
{
    float3 Position;
    uint Normal[2];
    uint Tangent[2];
    float4 Color;
    float2 TexCoords;
    float2 padding;
};

struct Mesh
{
    uint meshletOffset;
    uint vertexOffset;
    uint triangleOffset;
    uint vertexIndexOffset;
    float4x4 transform;
};

struct MeshIn
{
    [[vk::location(0)]] uint baseID : BASEID;
    [[vk::location(1)]] uint subIDs[32] : SUBID;
};

struct MeshOut
{
    float4 position : SV_Position;
    [[vk::location(0)]] float4 color : Color;
    [[vk::location(1)]] float3 fragPosition : FragPosition;
    [[vk::location(2)]] float3 normal : Normal;
    [[vk::location(3)]] float3 tangent : TANGENT;
    [[vk::location(4)]] float3 biTangent : BINORMAL;
    [[vk::location(5)]] float2 texCoord : TEXCOORD;
};

struct ComputeInput
{
    uint3 dispatchID : SV_DispatchThreadID; // gl_GlobalInvocationID
    uint3 groupID : SV_GroupID; // gl_WorkGroupID
    uint3 groupThreadID : SV_GroupThreadID; // gl_LocalInvocationID
    uint groupIndex : SV_GroupIndex;
};

struct MVPBufferObject
{
    float4x4 view;
    float4x4 proj;
    float4x4 mdl;
};


uint3 UnpackPrimitive(uint primitive)
{
    // Unpacks a 10 bits per index triangle from a 32-bit uint.
    return uint3(primitive & 0x3FF, (primitive >> 10) & 0x3FF, (primitive >> 20) & 0x3FF);
}

float3 GetRandomColor(
	uint _seed)
{
    uint hash = (_seed ^ 61) ^ (_seed >> 16);
    hash = hash + (hash << 3);
    hash = hash ^ (hash >> 4);
    hash = hash * 0x27d4eb2d;
    hash = hash ^ (hash >> 15);
    return float3(float(hash & 255),
				float((hash >> 8) & 255),
				float((hash >> 16) & 255)) / 255.0;
}




ConstantBuffer<MVPBufferObject> u_Mvp : register(b0, space0);
StructuredBuffer<Mesh> u_Meshes : register(t1, space0);

StructuredBuffer<Meshlet> u_Meshlets : register(t1, space1);
StructuredBuffer<Vertex> u_Vertices : register(t2, space1);
StructuredBuffer<uint> u_Triangles : register(t3, space1);
StructuredBuffer<uint> u_MeshletVertices : register(t4, space1);

uint GetVertexIndex(Meshlet meshlet, uint localIndex)
{
    return u_MeshletVertices[localIndex];
}

[numthreads(MS_GROUP_SIZE, 1, 1)]
[OutputTopology("triangle")]
void main(
    in payload MeshIn input, ComputeInput computeInput, // INPUT
    out vertices MeshOut vertsOut[MAX_VERTEX_COUNT], out indices uint3 primsOut[MAX_PRIMITIVE_COUNT] // OUTPUT
)
{
    const uint meshletVertInterations = ((MAX_VERTEX_COUNT + MS_GROUP_SIZE - 1) / MS_GROUP_SIZE);
    const uint meshletPrimIterations = ((MAX_PRIMITIVE_COUNT + MS_GROUP_SIZE - 1) / MS_GROUP_SIZE);
    const uint meshletIndex = input.baseID + input.subIDs[computeInput.groupID.x];
    const uint threadID = computeInput.groupThreadID.x;
    
    Meshlet meshlet = u_Meshlets[0];
    Mesh mesh = u_Meshes[0];
    
    const float4x4 mvp = mul(u_Mvp.proj, mul(u_Mvp.view, mesh.transform));
    
    uint triCount = meshlet.triangleCount + 1;
    uint vertCount = meshlet.vertexCount + 1;
    

    SetMeshOutputCounts(36, meshlet.triangleCount);
   
    [unroll]
    for (uint i = 0; i < 1; ++i)
    {
        uint vertID = computeInput.groupThreadID.x + i * MS_GROUP_SIZE;

        uint vertLoad = min(vertID, meshlet.vertexCount);
        
        uint vertexIndex = u_MeshletVertices[meshlet.vertexOffset + vertLoad];
        
        Vertex vertex = u_Vertices[vertexIndex];
        
        float4 fragPosition = mul(mvp, float4(vertex.Position, 1.0f));
        vertsOut[vertID].position = fragPosition;
        vertsOut[vertID].fragPosition = fragPosition.xyz;
        vertsOut[vertID].color = float4(GetRandomColor(meshletIndex), 1.f);
        vertsOut[vertID].texCoord = vertex.TexCoords;
		
        const float3 normal = DecodeNormalEncode(vertex.Normal);
        const float3 tangent = DecodeNormalEncode(vertex.Tangent);
        const float3 biTangent = cross(normal, tangent);
        float3x3 TBN = { tangent, biTangent, normal };

        vertsOut[vertID].normal = mul((float3x3) mesh.transform, normal);
        
    }

    [unroll]
    for (uint p = 0; p < meshletPrimIterations; ++p)
    {
        uint prim = computeInput.groupThreadID.x + p * MS_GROUP_SIZE;
        uint readPrim = min(prim, meshlet.triangleCount);

        uint3 indices = uint3(
        u_Triangles[meshlet.triangleOffset + readPrim * 3 + 0],
        u_Triangles[meshlet.triangleOffset + readPrim * 3 + 1],
        u_Triangles[meshlet.triangleOffset + readPrim * 3 + 2]);
        
        if (prim <= meshlet.triangleCount)
        {
            primsOut[prim] = indices;
        }
    }
}