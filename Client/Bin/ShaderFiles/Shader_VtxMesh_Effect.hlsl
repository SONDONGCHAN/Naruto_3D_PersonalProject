#include "Shader_Defines.hlsli"

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D   g_DiffuseTexture;
texture2D   g_DissolveTexture;
float2      g_UVMovement;
float       g_fAlpha;
float       g_fBrightness;
float4      g_vColor;
float       g_fDiscardColor;

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
    float2 vTexcoord2 : TEXCOORD2;    
    
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    
    float2 vTex = In.vTexcoord + g_UVMovement;

    Out.vTexcoord = vTex; 
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    return Out;
}

VS_OUT VS_DISSOLVE(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord + g_UVMovement;
    Out.vTexcoord2 = In.vTexcoord;   
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
    float2 vTexcoord2 : TEXCOORD2;
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

PS_OUT PS_SHOCK_WAVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    float4 vColor;
    float3 vRGB = float3(1.f, 1.f ,1.f);
    
    vColor.rgb = vRGB - vMtrlDiffuse.rgb;
    vColor.a = vMtrlDiffuse.a * g_fAlpha *3.f;
    
    Out.vDiffuse = vColor;
    
    return Out;
}

PS_OUT PS_KAMUI(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.2f)
        discard;

    float4 vColor;
    float3 vRGB = float3(1.f, 1.f, 1.f);
    
    vColor.rgb = vRGB - vMtrlDiffuse.rgb;
    vColor.a = vMtrlDiffuse.a * g_fAlpha ;
    
    Out.vDiffuse = vColor;
    
    return Out;
}

PS_OUT PS_RASENGUN_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    float4 vColor;
    float3 vRGB = float3(1.f, 1.f, 1.f);
    
    if (vMtrlDiffuse.r < 0.5)
    {
        vColor.rgb = vRGB - vMtrlDiffuse.rgb;
        vColor.rgb = vColor.rgb * g_vColor.rgb * g_fBrightness;
    }
    else
        vColor.rgb = float3(1.f, 1.f, 1.f);
    
    vColor.a = vMtrlDiffuse.a * g_vColor.a;
    
    Out.vDiffuse = vColor;
    

    return Out;
}

PS_OUT PS_RASENGUN_CHARGE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    float4 vColor = vMtrlDiffuse * g_vColor;
    
    Out.vDiffuse = vColor;
    
    return Out;
}
PS_OUT PS_RASENGUN_RUSH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    float4 vColor = vMtrlDiffuse * g_vColor;
    
    Out.vDiffuse = vColor;
  
    return Out;
}

PS_OUT PS_RASENGUN_BOOM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;
    

    vMtrlDiffuse.rgb = vMtrlDiffuse.rgb * g_vColor.rgb * g_fBrightness;
    vMtrlDiffuse.a = vMtrlDiffuse.a * g_vColor.a;
    
    Out.vDiffuse = vMtrlDiffuse;
    
    //Out.vDiffuse = g_vColor;
    return Out;
}
PS_OUT PS_RASENSHURIKEN_WING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f || vMtrlDiffuse.r < 0.5f)
        discard;

    float4 vColor;
    
    vColor = vMtrlDiffuse * g_vColor * g_fBrightness;
    
    Out.vDiffuse = vColor;
    

    return Out;
}

PS_OUT PS_RASENSHURIKEN_END(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    vector vDissolve    = g_DissolveTexture.Sample(g_LinearSampler, In.vTexcoord2);
     
    if (vMtrlDiffuse.a < 0.1f)
        discard;

    float4 vColor;
    float3 vRGB = float3(1.f, 1.f, 1.f);
    
    if (vMtrlDiffuse.r < 0.5)
    {
        vColor.rgb = vRGB - vMtrlDiffuse.rgb;
        vColor.rgb = vColor.rgb * g_vColor.rgb * g_fBrightness;
    }
    else
        vColor.rgb = float3(1.f, 1.f, 1.f);
    
    vColor.a = vMtrlDiffuse.a * g_vColor.a;
    
    if (vDissolve.r < g_fDiscardColor)
        discard;
    
    
    Out.vDiffuse = vColor;

    return Out;
}

PS_OUT PS_RASENSHURIKEN_DECO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f || vMtrlDiffuse.r < 0.6f)
        discard;
    
    //if (vMtrlDiffuse.a < 0.1f)
    //    discard;
    
    vMtrlDiffuse.rgb = vMtrlDiffuse.rgb * g_vColor.rgb;
    vMtrlDiffuse.a = vMtrlDiffuse.a * g_fAlpha * 2;
    
    Out.vDiffuse = vMtrlDiffuse;

    return Out;
}

PS_OUT PS_RASENSHURIKEN_DECO_END(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    vector vDissolve = g_DissolveTexture.Sample(g_LinearSampler, In.vTexcoord2);

    if (vMtrlDiffuse.a < 0.1f || vMtrlDiffuse.r < 0.6f)
        discard;
    
    vMtrlDiffuse.rgb = vMtrlDiffuse.rgb * g_vColor.rgb;
    vMtrlDiffuse.a = vMtrlDiffuse.a * g_fAlpha * 2;
    
    if (vDissolve.r < g_fDiscardColor)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;

    return Out;
}

PS_OUT PS_RASENGUN_SUPER_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_vColor;
    
    Out.vDiffuse = vMtrlDiffuse * g_fBrightness;
    
    //Out.vDiffuse = g_vColor;
    return Out;
}

PS_OUT PS_RASENGUN_SUPER_MAIN_END(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDissolve = g_DissolveTexture.Sample(g_LinearSampler, In.vTexcoord2);
    vector vMtrlDiffuse = g_vColor;
    
    if (vDissolve.r < g_fDiscardColor)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse * g_fBrightness;
    
    
    //Out.vDiffuse = g_vColor;
    return Out;
}

PS_OUT PS_WOOD_SWAP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
     

    vMtrlDiffuse.a = g_fAlpha;
    
    Out.vDiffuse = vMtrlDiffuse;
    

    return Out;
}

PS_OUT PS_KURAMA_SCRATCH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
     
    if (vMtrlDiffuse.a < 0.3f || vMtrlDiffuse.r < 0.3f )
        discard;
    
    vMtrlDiffuse *= (g_vColor * g_fBrightness);
    
    Out.vDiffuse = vMtrlDiffuse ;
    

    return Out;
}

PS_OUT PS_KURAMA_CLAW(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.3f || vMtrlDiffuse.r < 0.3f)
        discard;
    
     vMtrlDiffuse *= g_vColor;
    vMtrlDiffuse.a = g_fAlpha;
    
    Out.vDiffuse = vMtrlDiffuse;
    
    return Out;
}

PS_OUT PS_KURAMA_CRUSH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    vMtrlDiffuse *= g_vColor;
    vMtrlDiffuse.a = g_fAlpha;
    
    Out.vDiffuse = vMtrlDiffuse;
    
    return Out;
}

PS_OUT PS_CHECKPOINT_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.01)
        discard;
    
    vMtrlDiffuse *= (g_vColor * g_fBrightness);
    vMtrlDiffuse.a = g_fAlpha * g_vColor.a ;
    
    Out.vDiffuse = vMtrlDiffuse;
    
    return Out;
}

PS_OUT PS_CHECKPOINT_LINE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
    
    //if (vMtrlDiffuse.a < 0.1f)
    //    discard;
    
    Out.vDiffuse.rgb = g_vColor.rgb * g_fBrightness;
    Out.vDiffuse.a = g_fAlpha;
    
    
    return Out;
}

PS_OUT PS_CHIDORI(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    float4 vColor;
    
    vColor = vMtrlDiffuse * g_vColor * g_fBrightness;
    
    Out.vDiffuse = vColor;
    
    return Out;
}




technique11 DefaultTechnique
{
    pass FireBall_Main //0
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FIREBALL_MAIN();
    }

    pass FireBall_Ring //1
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FIREBALL_RING();
    }

    pass ShockWave //2
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SHOCK_WAVE();
    }

    pass Kamui //3
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_KAMUI();
    }

    pass RasengunMain //4
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RASENGUN_MAIN();
    }

    pass RasengunCharge //5
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RASENGUN_CHARGE();
    }

    pass RasengunRush //6
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RASENGUN_RUSH();
    }

    pass RasengunBoom //7
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RASENGUN_BOOM();
    }

    pass RasenShuriken_Wing //8
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RASENSHURIKEN_WING();
    }

    pass RasenShuriken_End //9
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_DISSOLVE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RASENSHURIKEN_END();
    }

    pass RasenShuriken_Deco //10
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RASENSHURIKEN_DECO();
    }

    pass RasenShuriken_Deco_End //11
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_DISSOLVE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RASENSHURIKEN_DECO_END();
    }

    pass RasengunSuper_Main //12
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RASENGUN_SUPER_MAIN();
    }

    pass RasengunSuper_Main_End //13
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_DISSOLVE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RASENGUN_SUPER_MAIN_END();
    }

    pass Wood_Swap //14
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WOOD_SWAP();
    }

    pass Kurama_Scratch //15
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_KURAMA_SCRATCH();
    }

    pass Kurama_Claw //16
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_KURAMA_CLAW();
    }

    pass Kurama_Crush //17
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_KURAMA_CRUSH();
    }

    pass CheckPoint_Main //18
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CHECKPOINT_MAIN();
    }
    
    pass CheckPoint_Line //19
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CHECKPOINT_LINE();
    }

    pass Chidori_Main //20
    {
        SetRasterizerState(RS_None_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CHIDORI();
    }

}



