
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

PixelInput main(VertexInput input)
{
	PixelInput output;
	output.position = input.position;
	output.color = input.color;
	return output;
}