
#ifndef GROUP_SIZE
#define GROUP_SIZE 32
#endif 


struct Meshlet
{
    uint vertexOffset;
    uint triangleOffset;
    uint vertexCount;
    uint triangleCount;
    uint meshId;
    uint padd[3];
};

[[vk::binding(0, 1)]] StructuredBuffer<Meshlet> u_Meshlets : register(t0, space1);


struct TaskOutput
{
    uint baseID;
    uint subID[GROUP_SIZE];
};

struct TaskInput
{
    uint3 dispatchID : SV_DispatchThreadID; // gl_GlobalInvocationID
    uint3 groupID : SV_GroupID; // gl_WorkGroupID
    uint3 groupThreadID : SV_GroupThreadID; // gl_LocalInvocationID
};

struct Constant
{
    uint meshletCount;
    uint vertexCount;
    uint indexCount;
    uint renderDebugState;
};

[[vk::push_constant]] Constant u_Constant;
groupshared TaskOutput output;
[numthreads(GROUP_SIZE, 1, 1)]
void main(in TaskInput input)
{
    Meshlet meshlet = u_Meshlets[input.dispatchID.x];

    bool render = input.dispatchID.x < u_Constant.meshletCount;
    
    uint tasks = WaveActiveCountBits(render);

    if (input.groupThreadID.x == 0)
    {
    // write the number of surviving meshlets, i.e. 
    // mesh workgroups to spawn
        output.baseID = input.groupID.x * GROUP_SIZE;


    // where the meshletIDs started from for this task workgroup
    }
    if (render)
    {
        uint idxOffset = WavePrefixCountBits(render);
        output.subID[idxOffset] = uint(input.groupThreadID.x);
    }
    DispatchMesh(tasks, 1, 1, output);
    
}