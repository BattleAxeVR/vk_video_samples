struct VSInput
{
[[vk::location(0)]] float2 Pos : POSITION0;
[[vk::location(1)]] float2 UV : TEXCOORD0;
};

layout(location = 0) out vec2 vTexCoord;

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
	float2 vTexCoord : TEXCOORD0;
};


VSOutput main(VSInput input)
{
	VSOutput output = (VSOutput)0;
    output.vTexCoord = transformPushConstants.texMatrix * input.UV;
    output.position = vec4(input.Pos, 0, 1);
	
	return output;
}
