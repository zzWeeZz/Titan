
struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
    float2 TexCoord : TEXCOORD;
};

Texture2D test : register(t0);
SamplerState borderSampler : register(s0);

float4 main(PixelInput input) : SV_Target
{
	return test.Sample(borderSampler, input.TexCoord);
}