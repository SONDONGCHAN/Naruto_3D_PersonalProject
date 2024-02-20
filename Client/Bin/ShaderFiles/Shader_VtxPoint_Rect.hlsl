#include "Shader_Defines.hlsli"

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D   g_Texture;
float2      vSize;
float       g_fRatio = 1.f;
vector      g_vColor;


struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition        : POSITION;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    Out.vPosition       = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    
    return Out;
}

struct GS_IN
{
    float4 vPosition        : POSITION;
};

struct GS_OUT
{
    float4 vPosition    : SV_POSITION0;
    float2 vTexcoord    : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];

	matrix matVP    = mul(g_ViewMatrix, g_ProjMatrix);
    float3 vRight   = float3(0.5f, 0.f, 0.f);
    float3 vup      = float3(0.f, 0.5f, 0.f);
     
    Out[0].vPosition = In[0].vPosition - float4(vRight * vSize.x, 0.f) + float4(vup * vSize.y, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord  = float2(0.f, 0.f);
    

    Out[1].vPosition = In[0].vPosition + float4(vRight * vSize.x, 0.f) + float4(vup * vSize.y, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord  = float2(1.f, 0.f);
    

    Out[2].vPosition = In[0].vPosition + float4(vRight * vSize.x, 0.f) - float4(vup * vSize.y, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord  = float2(1.f, 1.f);
    

    Out[3].vPosition = In[0].vPosition - float4(vRight * vSize.x, 0.f) - float4(vup * vSize.y, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord  = float2(0.f, 1.f);  

    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.RestartStrip();

    DataStream.Append(Out[0]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
}

[maxvertexcount(6)]
void GS_HP(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    float3 vRight   = float3(0.5f, 0.f, 0.f);
    float3 vup      = float3(0.f, 0.5f, 0.f);
    
    
    Out[0].vPosition = In[0].vPosition - float4(vRight * vSize.x, 0.f) + float4(vup * vSize.y, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    
    
    Out[1].vPosition = In[0].vPosition + float4(vRight * vSize.x, 0.f) + float4(vup * vSize.y, 0.f) - float4(vRight * 2 * vSize.x * (1.f - g_fRatio), 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    
    
    Out[2].vPosition = In[0].vPosition + float4(vRight * vSize.x, 0.f) - float4(vup * vSize.y, 0.f) - float4(vRight * 2 * vSize.x * (1.f - g_fRatio), 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    
    
    Out[3].vPosition = In[0].vPosition - float4(vRight * vSize.x, 0.f) - float4(vup * vSize.y, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);

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
    float4 vPosition    : SV_POSITION0;  
    float2 vTexcoord    : TEXCOORD0;    
    
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    
};

PS_OUT PS_STATIC(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord);
	
    if (vColor.a < 0.1f)
        discard;
    
    Out.vColor = vColor;
    
    return Out;
}

PS_OUT PS_HP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord);
   	
    if (vColor.a < 0.6f)
        discard;
    
    vColor = g_vColor;
    
    Out.vColor = vColor;
   
    return Out;
}

PS_OUT PS_SKILL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
      
    vector vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord);
    
    if (vColor.a < 0.1f)
        discard;
    
    vector vFinalColor = (In.vTexcoord.y > g_fRatio ) ? vColor : vColor * 0.3f;
    
    Out.vColor = vFinalColor;
    
    return Out;
}

PS_OUT PS_WORLD_HP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord);
   	
    if (vColor.a < 0.1f)
        discard;
    
   // vColor = g_vColor;
    
    Out.vColor = vColor;
   
    return Out;
}

/* EffectFramework */

technique11 DefaultTechnique
{
    pass Static_Center
    {
		/* RenderState설정. */
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_STATIC();
    }

    pass Dynamic_Hp
    {
		/* RenderState설정. */
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_HP();
        PixelShader = compile ps_5_0 PS_HP();
    }

    pass Dynamic_Skill
    {
		/* RenderState설정. */
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_SKILL();
    }

    pass Dynamic_Hp_World
    {
		/* RenderState설정. */
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_HP();
        PixelShader = compile ps_5_0 PS_WORLD_HP();
    }
}


