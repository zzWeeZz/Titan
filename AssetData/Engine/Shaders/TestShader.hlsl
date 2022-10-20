struct VertexInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct PixelInput
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

struct ObjectBuffer
{
    float4x4 view;
    float4x4 proj;
    float4x4 mdlSpace;
    float4x4 padding;
};

ConstantBuffer<ObjectBuffer> objectBuffer : register(b0, space0);

PixelInput main(VertexInput input)
{
    PixelInput output;
    float4x4 mvp = mul(objectBuffer.proj, mul(objectBuffer.view, objectBuffer.mdlSpace));
    output.Position = mul(mvp, float4(input.Position, 1));
    output.Color = float4(input.TexCoord.x, input.TexCoord.y, 0, 1);
    output.TexCoord = input.TexCoord;
    return output;
}