#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix      g_ViewMatrixInv, g_ProjMatrixInv;

texture2D	g_Texture;

float g_fTexW = 1280.0f;
float g_fTexH = 720.0f;

vector      g_vCamPosition;

texture2D   g_ShadeTexture;
texture2D   g_DepthTexture;
texture2D   g_SpecularTexture;


texture2D   g_EffectTexture;
texture2D   g_BlurTexture;
texture2D   g_BackBufferTexture;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}


struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector      vBackBuffer = g_BackBufferTexture.Sample(g_LinearSampler, In.vTexcoord);

    vector      vEffect     = g_EffectTexture.Sample(g_LinearSampler, In.vTexcoord);
	vector		vBlur       = g_BlurTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    vEffect.rgb *= 0.7f;
    
    Out.vColor = vBackBuffer + vBlur + vEffect;
  
    return Out;
}


technique11 DefaultTechnique
{
    pass PostProcessing_Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }
}