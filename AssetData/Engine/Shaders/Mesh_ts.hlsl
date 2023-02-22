
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

StructuredBuffer<Meshlet> u_Meshlets : register(b0, space1);


struct TaskOutput
{
    uint baseID;
    uint subID[GROUP_SIZE];
};

struct TaskInput
{
    uint dispatchID : SV_DispatchThreadID;
    uint groupID : SV_GroupID;
    uint groupThreadID : SV_GroupThreadID;
};

struct Constant
{
    uint meshletCount;
    uint vertexCount;
    uint indexCount;
    uint renderDebugState;
};

[[vk::push_constant]] Constant u_Constant;

[numthreads(GROUP_SIZE, 1, 1)]
TaskOutput main(in TaskInput input)
{
    TaskOutput output = (TaskOutput) 0;
    Meshlet meshlet = u_Meshlets[input.dispatchID.x];

    bool render = input.dispatchID.x < u_Constant.meshletCount;
    
    uint4 vote = WaveActiveBallot(render);
    uint tasks = subgroupBallotBitCount(vote);

    if (input.groupThreadID.x == 0)
    {
    // write the number of surviving meshlets, i.e. 
    // mesh workgroups to spawn
        DispatchMesh(tasks, 1u, 1u);

    // where the meshletIDs started from for this task workgroup
        output.baseID = input.groupID.x * GROUP_SIZE;
    }

    uint idxOffset = subgroupBallotExclusiveBitCount(vote);
    if (render)
    {
        output.subID[idxOffset] = uint(input.groupThreadID.x);
    }
    return output;
}