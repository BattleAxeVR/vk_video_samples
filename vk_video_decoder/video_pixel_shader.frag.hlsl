Texture2D textureColor : register(t0);
SamplerState samplerColor : register(s0);

struct VSOutput
{
[[vk::location(0)]] float2 UV : TEXCOORD0;
};

float4 main(VSOutput input) : SV_TARGET
{
	float4 colour = textureColor.SampleLevel(samplerColor, input.UV, 0.0);
	return colour;
}