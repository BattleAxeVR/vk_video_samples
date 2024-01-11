struct VSInput
{
[[vk::location(0)]] float2 position : POSITION0;
[[vk::location(1)]] float2 UV : TEXCOORD0;
};

struct VSPushConstants
{
    float4x4 posMatrix;
    float2x2 texMatrix;
};

[[vk::push_constant]]
VSPushConstants constants;

struct VSOutput
{
	float4 position : SV_POSITION;
	float2 UV : TEXCOORD0;
};


VSOutput main(VSInput input)
{
	VSOutput output = (VSOutput)0;
    output.UV = input.UV;
	//output.UV = mul(constants.texMatrix, input.UV);
    output.position = float4(input.position, 0, 1);
	
	return output;
}
