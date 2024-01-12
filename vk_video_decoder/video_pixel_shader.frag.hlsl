Sampler2D tex;

struct VSOutput
{
[[vk::location(0)]] float2 UV : TEXCOORD0;
};

float4 main(VSOutput input) : SV_TARGET
{
	//float4 colour = tex.Sample(input.UV);
	float4 colour = tex.SampleLevel(input.UV, 0.0);
	//float4 colour = tex.SampleGrad(input.UV, float2(0,0), float2(0,0));//, 0, 0.5f);
	
	return colour;
}
