#version 450
#extension GL_NV_mesh_shader: require
#extension GL_KHR_shader_subgroup_ballot : require
#define GROUP_SIZE 32
layout (local_size_x = GROUP_SIZE) in;

struct Meshlet
{
  uint vertexOffset;
  uint triangleOffset;
  uint vertexCount;
  uint triangleCount;
  uint meshId;
	uint padd[3];
};

layout (std430, binding = 0, set = 1) readonly buffer globalMeshlets
{
  Meshlet meshlets[];
};

taskNV out Task
{
  uint   baseID;
  uint   subIDs[GROUP_SIZE];
} OUT;

layout (push_constant) uniform constants
{
    uint meshletCount;
		uint vertexCount;
		uint indexCount;
		uint renderDebugState;
};

void main()
{
    Meshlet meshlet = meshlets[gl_GlobalInvocationID.x];

    bool render = gl_GlobalInvocationID.x < meshletCount;
    
    uvec4 vote  = subgroupBallot(render);
    uint  tasks = subgroupBallotBitCount(vote);

    if (gl_LocalInvocationID.x == 0)
    {
    // write the number of surviving meshlets, i.e. 
    // mesh workgroups to spawn
        gl_TaskCountNV = tasks;

    // where the meshletIDs started from for this task workgroup
        OUT.baseID = gl_WorkGroupID.x * GROUP_SIZE;
    }

    uint idxOffset = subgroupBallotExclusiveBitCount(vote);
    if (render)
    {
        OUT.subIDs[idxOffset] = uint(gl_LocalInvocationID.x);
    }
}