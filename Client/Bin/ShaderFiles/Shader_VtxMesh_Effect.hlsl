#include "Shader_Defines.hlsli"

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D   g_DiffuseTexture;
float2      g_UVMovement;
float       g_fAlpha;
float       g_fBrightness;
float4      g_vColor;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;   
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord + g_UVMovement;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
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
};

PS_OUT PS_FIREBALL_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    vMtrlDiffuse.rgb = saturate(vMtrlDiffuse.rgb * g_fBrightness);
    
    Out.vDiffuse = vector(vMtrlDiffuse.rgb, g_fAlpha);
    //Out.vDiffuse = vector(1.f, 0.f, 0.f, 1.f);
    
    return Out;
}

PS_OUT PS_FIREBALL_RING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    float4 vColor;
    
    if (vMtrlDiffuse.r < 0.8f)
        vColor.rgb = g_vColor.rgb * (1.f - vMtrlDiffuse.r);
    else
        vColor.rgb = vMtrlDiffuse.rgb;
    
    vColor.rgb  = saturate(vColor.rgb * g_fBrightness);
    vColor.a    = g_vColor.a;
    
    Out.vDiffuse = vColor;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass FireBall_Main
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FIREBALL_MAIN();
    }

    pass FireBall_Ring
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FIREBALL_RING();
    }
}


