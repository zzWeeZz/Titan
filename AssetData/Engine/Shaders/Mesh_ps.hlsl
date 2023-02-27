

struct MeshOutput
{
    [[vk::location(0)]] float4 color : COLOR;
    [[vk::location(1)]] float3 fragPosition : WORLDPOSITION;
    [[vk::location(2)]] float3 normal : NORMAL;
    [[vk::location(3)]] float3 tangent : TANGENT;
    [[vk::location(4)]] float3 biTangent : BINORMAL;
    [[vk::location(5)]] float2 texCoord : TEXCOORD;
};


[[vk::combinedImageSampler]] Texture2D<float4> u_Albedo : register(t5, space0);

[[vk::combinedImageSampler]] SamplerState u_AlbedoSampler : register(s5, space0);


struct LightData
{
    float4 direction;
    float4 color;
};

ConstantBuffer<LightData> u_LightData : register(b6, space0);


struct Constant
{
    uint meshletCount;
    uint vertexCount;
    uint indexCount;
    uint renderDebugState;
};

[[vk::push_constant]] Constant u_Constant;

float4 main(MeshOutput input) : SV_Target
{
    float lightStrength = max(dot(input.normal, u_LightData.direction.xyz), 0.0f);
    
    float3 lightValue = lightStrength * (u_LightData.color.xyz * u_LightData.color.w);
    
    float4 albedo = u_Albedo.Sample(u_AlbedoSampler, input.texCoord);
    
    if (u_Constant.renderDebugState > 0)
    {
        albedo = input.color;
        
        return pow(albedo, float4(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
    }
    
    albedo.xyz = (float3(0.3, 0.3, 0.3) + lightValue * albedo.xyz);
    
    return pow(albedo, float4(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
}