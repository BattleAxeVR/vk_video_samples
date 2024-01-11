Texture2D<float4> tex00[1] : register(t0);
SamplerState _tex00_sampler[1] : register(s0);

static float4 oFrag;
static float2 vTexCoord;

struct SPIRV_Cross_Input
{
    float2 vTexCoord : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 oFrag : SV_Target0;
};

void frag_main()
{
    oFrag = tex00[0].Sample(_tex00_sampler[0], vTexCoord);
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    vTexCoord = stage_input.vTexCoord;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.oFrag = oFrag;
    return stage_output;
}
