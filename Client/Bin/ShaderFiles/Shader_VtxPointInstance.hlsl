#include "Shader_Defines.hlsli"

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector      g_vCenterPos;
texture2D   g_Texture;
vector      g_vCamPosition;
float2      g_vSpriteRatio;
float2      g_vSpriteCurrnetRatio;


struct VS_IN
{
    float3 vPosition : POSITION;
	float2 vPSize : PSIZE;
  
    row_major float4x4 TransformMatrix : WORLD;
	float4  vColor       : COLOR0;
};

struct VS_OUT
{
    float4 vPosition    : POSITION;
    float2 vPSize       : PSIZE;
	float4 vColor       : COLOR0;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    vector vPosition = mul(vector(In.vPosition, 1.f), In.TransformMatrix);
	
    Out.vPosition = mul(vPosition, g_WorldMatrix);

    Out.vPSize = float2(length(In.TransformMatrix._11_12_13) * In.vPSize.x, 
                        length(In.TransformMatrix._21_22_23) * In.vPSize.y);
    
    Out.vColor = In.vColor;

    return Out;
}


struct GS_IN
{
    float4 vPosition    : POSITION;
    float2 vPSize       : PSIZE;
    
    float4 vColor       : COLOR0;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vColor : COLOR0;
};

// void GS_MAIN(point GS_IN[1], inout LineStream<GS_OUT> DataStream)
// void GS_MAIN(triangle GS_IN[3], inout TriangleStream<GS_OUT> DataStream)
// void GS_MAIN(line GS_IN[2], inout TriangleStream<GS_OUT> DataStream)
[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];

    float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;   
    float3 vUp = normalize(In[0].vPosition - g_vCenterPos) * In[0].vPSize.y;  
    float3 vRight = normalize(cross(vLook, vUp)) * In[0].vPSize.x * 0.5f;
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = In[0].vPosition + float4(vRight, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vColor = In[0].vColor;

    Out[1].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vColor = In[0].vColor;

    Out[2].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vColor = In[0].vColor;

    Out[3].vPosition = In[0].vPosition - float4(vRight, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vColor = In[0].vColor;
    
    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.RestartStrip();

    DataStream.Append(Out[0]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
    

}

[maxvertexcount(6)]
void GS_NONE_DIRECTION(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];

    float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vColor = In[0].vColor;

    Out[1].vPosition = In[0].vPosition - float4(vRight, 0.f) + float4(vUp, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vColor = In[0].vColor;

    Out[2].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vColor = In[0].vColor;

    Out[3].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vColor = In[0].vColor;
    
    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.RestartStrip();

    DataStream.Append(Out[0]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
}


struct PS_IN
{
    float4 vPosition    : SV_POSITION;
    float2 vTexcoord    : TEXCOORD0;
    float4 vColor       : COLOR0;
    
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_Texture.Sample(g_LinearSampler, In.vTexcoord);
    
        if (vMtrlDiffuse.a < 0.01f)
        discard;
        
    float4 vColor;
    
    if (vMtrlDiffuse.a < 0.8f)
        vColor.rgb = In.vColor.rgb * (1.f - vMtrlDiffuse.a);
    else
        vColor.rgb = vMtrlDiffuse.rgb;
    
    vColor.a = In.vColor.a;
 
    Out.vColor = vColor;
    //float4 vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
    
    //if (vColor.a < 0.3f)
    //    discard;
        
    //vColor = In.vColor;
    
    //Out.vColor = vColor;
  
    return Out;
}

PS_OUT PS_NONE_DIRECTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //float4 vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;
    
    float2 SpritePos;
    SpritePos.x = (In.vTexcoord.x / g_vSpriteRatio.x) + (g_vSpriteCurrnetRatio.x * (1 / g_vSpriteRatio.x));
    SpritePos.y = (In.vTexcoord.y / g_vSpriteRatio.y) + (g_vSpriteCurrnetRatio.y * (1 / g_vSpriteRatio.y));
    
    float4 Diffuse = g_Texture.Sample(g_LinearSampler, SpritePos) * In.vColor;
    
    if (Diffuse.a < 0.3f)
        discard;
    
    float4 vColor;
    
    
    if (Diffuse.a < 0.8f)
        vColor.rgb = In.vColor.rgb * (1.f - Diffuse.a);
    else
        vColor.rgb = Diffuse.rgb;
    
    vColor.a = In.vColor.a;
 
    Out.vColor = vColor;
    
        
    //vColor.rgb = vColor.rgb * In.vColor.r;
    
    //Out.vColor = vColor;
  
    return Out;
}

/* EffectFramework */

technique11 DefaultTechnique
{
    pass Directional
    {
		/* RenderState설정. */
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass None_Direction
    {
		/* RenderState설정. */
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_NONE_DIRECTION();
        PixelShader = compile ps_5_0 PS_NONE_DIRECTION();
    }

}


