
struct VertexInput
{
	float4 position : POSITION;
	float4 color : COLOR;
};

float4 main(VertexInput input) : SV_POSITION
{
	return input.position;
}