Sampler2D tex;

struct VSOutput
{
[[vk::location(0)]] float2 UV : TEXCOORD0;
};

float4 main(VSOutput input) : SV_TARGET
{
	float4 colour = tex.Sample(input.UV);
	return colour;
}
