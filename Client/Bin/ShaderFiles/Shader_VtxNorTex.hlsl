#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

texture2D g_DiffuseTexture[2];
texture2D g_MaskTexture;
texture2D g_BrushTexture;

vector g_vBrushPos;
float g_fBrushRangeX = 0.f;
float g_fBrushRangeZ = 0.f;


vector g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);

vector g_vLightPos = vector(30.f, 10.0f, 30.f, 1.f);
float g_fLightRange = 30.f;

vector g_vCamPosition;


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
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
    Out.vTexcoord = In.vTexcoord;
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
    vector vColor : SV_TARGET0;
};


PS_OUT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture[0].Sample(g_LinearSampler, In.vTexcoord * 30.f);

    vector vLightDir = In.vWorldPos - g_vLightPos;

    float fDistance = length(vLightDir);

    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

    vector vReflect = reflect(normalize(vLightDir), normalize(In.vNormal));
    vector vLook = In.vWorldPos - g_vCamPosition;

	/* 0 ~ 1 */
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f);
    float fShade = max(dot(normalize(vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

    Out.vColor = ((g_vLightDiffuse * vMtrlDiffuse) * min(fShade + (g_vLightAmbient * g_vMtrlAmbient), 1.f)
		+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular) * fAtt;

    return Out;
}

PS_OUT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vSourDiffuse = g_DiffuseTexture[0].Sample(g_LinearSampler, In.vTexcoord * 30.f);
    vector vDestDiffuse = g_DiffuseTexture[1].Sample(g_LinearSampler, In.vTexcoord * 30.f);
    vector vMask = g_MaskTexture.Sample(g_PointSampler, In.vTexcoord);
    vector vBrush = vector(0.f, 0.f, 0.f, 0.f);

    if (g_vBrushPos.x - g_fBrushRangeX < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRangeX &&
		g_vBrushPos.z - g_fBrushRangeZ < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRangeZ)
    {
        float2 vBrushUV;

        vBrushUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRangeX)) / (2.f * g_fBrushRangeX);
        vBrushUV.y = ((g_vBrushPos.z + g_fBrushRangeZ) - In.vWorldPos.z) / (2.f * g_fBrushRangeZ);

        vBrush = g_BrushTexture.Sample(g_LinearSampler, vBrushUV);
    }

    vector vMtrlDiffuse = vDestDiffuse * vMask + vSourDiffuse * (1.f - vMask) + vBrush;
	
    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector vLook = In.vWorldPos - g_vCamPosition;

	/* 0 ~ 1 */
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f);
    float fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

    Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * min(fShade + (g_vLightAmbient * g_vMtrlAmbient), 1.f)
		+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

    return Out;
}


technique11 DefaultTechnique
{
    pass Lighting_Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Lighting_Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

}


