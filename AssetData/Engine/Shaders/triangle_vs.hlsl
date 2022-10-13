
struct VertexInput
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

cbuffer ColorBuffer : register(b0)
{
    float4x4 view;
    float4x4 proj;
}

PixelInput main(VertexInput input)
{
	PixelInput output;
	
	output.position = input.position;
	output.color = input.color;
	return output;
}