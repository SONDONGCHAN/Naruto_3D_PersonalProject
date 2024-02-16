#include "Shader_Defines.hlsli"

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix      g_WorldMatrix_Old;
vector      g_vPosition_Old;
vector      g_vColor;



struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition        : POSITION0;
    float4 vPosition_Old    : POSITION1;
    
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    Out.vPosition       = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vPosition_Old   = mul(vector(In.vPosition, 1.f), g_WorldMatrix_Old);
    
    return Out;
}


struct GS_IN
{
    float4 vPosition        : POSITION0;
    float4 vPosition_Old    : POSITION1;

};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT Out[4];

    float3 vLook    = (In[0].vPosition_Old - In[0].vPosition).xyz;
    float3 vRight   = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) ;
    float3 vUp      = normalize(cross(vLook, vRight));
	matrix matVP    = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = In[0].vPosition - float4(vRight * 0.02f, 0.f) + float4(vLook + normalize(vLook) * 0.01f, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);

    Out[1].vPosition = In[0].vPosition + float4(vRight * 0.02f, 0.f) + float4(vLook + normalize(vLook) * 0.01f, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);

    Out[2].vPosition = In[0].vPosition + float4(vRight * 0.02f, 0.f) ;
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);

    Out[3].vPosition = In[0].vPosition - float4(vRight * 0.02f, 0.f) ;
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
    float4 vPosition    : SV_POSITION;
    float2 vTexcoord    : TEXCOORD0;    
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_vColor;
    
    //Out.vColor = vector(113.f/255.f, 199.f/255.f, 236.f/255.f, g_fAlpha);
  
    return Out;
}

/* EffectFramework */

technique11 DefaultTechnique
{
    pass DefaultPass
    {
		/* RenderState¼³Á¤. */
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}


