#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_Texture;

vector		g_vLightDir;
vector      g_vLightDiffuse;
vector      g_vLightAmbient;

vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

texture2D	g_NormalTexture;
texture2D   g_DiffuseTexture;
texture2D   g_ShadeTexture;

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


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord);

	return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(g_PointSampler, In.vTexcoord);

	/* 0 ~ 1 -> -1 ~ 1 */
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    Out.vShade = g_vLightDiffuse * 
                saturate(saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) 
                         + (g_vLightAmbient * g_vMtrlAmbient));

    return Out;
}
PS_OUT PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    if (0.0f == vDiffuse.a)
        discard;

    vector vShade = g_ShadeTexture.Sample(g_LinearSampler, In.vTexcoord);

    Out.vColor = vDiffuse * vShade;

    return Out;
}

technique11	DefaultTechnique
{
    pass Debug
    {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass Light_Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }
	
    pass Light_Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }
    pass Deferred_Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }

}


