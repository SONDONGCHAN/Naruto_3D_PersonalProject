#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;

float g_fAlphaDiscard = 0.f;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal   : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent  : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal   : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};



VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matVP;

    matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    vector vPosition = vector(In.vPosition, 1.f);
    vector vNormal = vector(In.vNormal, 0);
    vNormal = normalize(mul(vNormal, g_WorldMatrix));
    vPosition = mul(vPosition, g_WorldMatrix) + (vNormal * 0.03f);
    
    Out.vPosition = mul(vPosition, matVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vNormal = vNormal;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
	vector vNormal : SV_TARGET1;
    
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
	
    if (vMtrlDiffuse.a < g_fAlphaDiscard)
        discard;

    vMtrlDiffuse = vector(0.f, 0.f, 0.f, 1.f);
    
    Out.vDiffuse = vMtrlDiffuse;
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    return Out;
}


technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Front_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}


