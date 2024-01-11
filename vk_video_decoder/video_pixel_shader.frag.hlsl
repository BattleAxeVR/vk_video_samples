[[vk::binding(0, 0)]]
Texture2D textureColor;

//Texture2D textureY;// : register(t0);
//Texture2D textureUV;// : register(t0);

// slangc complains if here is no vk::binding decorator. but no matter what value I set, the output is red, so it's not using the immutable sampler, but some other default one
// see : https://github.com/KhronosGroup/Vulkan-Docs/issues/985

//[[vk::binding(1, 0)]]
SamplerState samplerColor;// : register(s0);

struct VSOutput
{
[[vk::location(0)]] float2 UV : TEXCOORD0;
};

float4 main(VSOutput input) : SV_TARGET
{
	float4 colour = textureColor.Sample(samplerColor, input.UV);

	//float4 colour = textureColor.SampleLevel(samplerColor, input.UV, 0.0);
	//float4 colour = textureY.Sample(samplerColor, input.UV);
	//colour.gb = textureUV.Sample(samplerColor, input.UV).rg;
	
	return colour;
}