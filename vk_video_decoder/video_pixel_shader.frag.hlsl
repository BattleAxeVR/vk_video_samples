[[vk::combinedImageSampler]]
Texture2D<float4> textureColor : register(t0);

[[vk::combinedImageSampler]]
SamplerState samplerColor : register(t0);

struct VSOutput
{
[[vk::location(0)]] float2 UV : TEXCOORD0;
};

float4 main(VSOutput input) : SV_TARGET
{
	float4 colour = textureColor.Sample(samplerColor, input.UV);
	return colour;
}