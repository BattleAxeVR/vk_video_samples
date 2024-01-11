[[vk::binding(0, 0)]]
Texture2D textureColor : register(t0);

[[vk::binding(1, 0)]]
SamplerState samplerColor : register(s0);

struct VSOutput
{
[[vk::location(0)]] float2 UV : TEXCOORD0;
};

float4 main(VSOutput input) : SV_TARGET
{
	float4 colour = textureColor.Sample(samplerColor, input.UV);
	return colour;
}
