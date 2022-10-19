
struct VertexInput
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
    float2 TexCoord : TEXCOORD;
};

cbuffer ColorBuffer
{
    float4x4 view;
    float4x4 proj;
    float4x4 mdlSpace;
    float4x4 padding;
}

ConstantBuffer<ColorBuffer> objectBuffer : register(b0);


PixelInput main(VertexInput input)
{
	PixelInput output;
    float4x4 mvp = mul(objectBuffer.proj, mul(objectBuffer.view, objectBuffer.mdlSpace));
    output.position = mul(mvp, float4(input.Position, 1));
    output.color = float4(input.TexCoord.x, input.TexCoord.y, 0, 1);
    output.TexCoord = input.TexCoord;
	return output;
}