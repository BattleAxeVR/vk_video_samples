cbuffer constants
{
    row_major float4x4 transformPushConstants_posMatrix : packoffset(c0);
    row_major float2x2 transformPushConstants_texMatrix : packoffset(c4);
};


static float4 gl_Position;
static float2 vTexCoord;
static float2 aTexCoord;
static float2 aVertex;

struct SPIRV_Cross_Input
{
    float2 aVertex : POSITION;
    float2 aTexCoord : NORMAL;
};

struct SPIRV_Cross_Output
{
    float2 vTexCoord : TEXCOORD0;
    float4 gl_Position : SV_Position;
};

void vert_main()
{
    vTexCoord = mul(aTexCoord, transformPushConstants_texMatrix);
    gl_Position = float4(aVertex, 0.0f, 1.0f);
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    aTexCoord = stage_input.aTexCoord;
    aVertex = stage_input.aVertex;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.vTexCoord = vTexCoord;
    return stage_output;
}
