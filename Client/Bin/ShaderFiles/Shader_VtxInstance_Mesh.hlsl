#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D   g_Texture, g_DesolveTexture;
float3      g_StartColor, g_EndColor, g_DesolveColor, g_BloomColor;
bool		g_Desolve, g_Alpha, g_Color;
float		g_BlurPower, g_DesolvePower;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3      vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3      vTangent : TANGENT;

    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
    float2 vLifeTime : COLOR0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;

    float2 vLifeTime : COLOR0;
};

/* 정점 셰이더 :  /* 
/* 1. 정점의 위치 변환(월드, 뷰, 투영).*/
/* 2. 정점의 구성정보를 변경한다. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix TransformMatrix = matrix(In.vRight, In.vUp, In.vLook, In.vTranslation);

    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;

    Out.vLifeTime = In.vLifeTime;
    
    

    return Out;
}

/* TriangleList인 경우 : 정점 세개를 받아서 w나누기를 각 정점에 대해서 수행한다. */
/* 뷰포트(윈도우좌표로) 변환. */
/* 래스터라이즈 : 정점으로 둘러쌓여진 픽셀의 정보를, 정점을 선형보간하여 만든다. -> 픽셀이 만들어졌다!!!!!!!!!!!! */


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : COLOR0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};


PS_OUT PS_MAIN_SPREAD(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    float4 vNoise = g_DesolveTexture.Sample(LinearSampler, In.vTexcoord);
    if (Out.vColor.a < 0.1f)
        discard;

    float fRatio = In.vLifeTime.y / In.vLifeTime.x;
    if (g_Alpha)
    {
        Out.vColor.a *=  1- fRatio;
    }
   
    if (g_Color)
    {
        Out.vColor.rgb = lerp(g_StartColor, g_EndColor, fRatio);
    }

    if (g_Desolve)
    {
        if ((vNoise.r - fRatio) < g_DesolvePower * 0.01f)
        {
            Out.vColor.rgb = g_DesolveColor;
        }

        if (vNoise.r < fRatio)
        {
            discard;
        }
    }
    return Out;
}

PS_OUT PS_BLOOM(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    float4 vNoise = g_DesolveTexture.Sample(LinearSampler, In.vTexcoord);
    float fRatio = In.vLifeTime.y / In.vLifeTime.x;

    if (g_Alpha)
    {
        Out.vColor.a = g_BlurPower * (1- fRatio);
    }
    else
    {
        Out.vColor.a = g_BlurPower;
    }
    Out.vColor.rgb = g_BloomColor;

    if (g_Desolve)
    {
        if ((vNoise.r - fRatio) < g_DesolvePower * 0.01f)
        {
            Out.vColor.rgb = g_DesolveColor;
        }

        if (vNoise.r < fRatio)
        {
            discard;
        }
    }
    return Out;
}

PS_OUT PS_BLADE(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    float fRatio = In.vLifeTime.y / In.vLifeTime.x;
    float2 adjustedUV = In.vTexcoord;
    if (fRatio <= 0.2)
    {
        // fRatio를 0-1 범위로 정규화
        float normalizedRatio = fRatio / 0.2;

        // U 좌표를 시간에 따라 조정
        adjustedUV.x = lerp(-1.0, In.vTexcoord.x, normalizedRatio);
    }
    if (adjustedUV.x >= 0.0 && adjustedUV.x <= 1.0)
    {
        Out.vColor = g_Texture.Sample(LinearSampler, adjustedUV);
    }
    else
    {
        // UV가 유효 범위를 벗어나면 투명하게 처리
        Out.vColor = float4(0, 0, 0, 0);
    }

    //Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    float4 vNoise = g_DesolveTexture.Sample(LinearSampler, adjustedUV);
    if (Out.vColor.a < 0.1f)
        discard;

    Out.vColor.a *= (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.0f;

    if (g_Alpha)
    {
        Out.vColor.a *= 1 - fRatio;
    }

    if (g_Color)
    {
        Out.vColor.rgb = lerp(g_StartColor, g_EndColor, fRatio);
    }

    if (g_Desolve)
    {
        if ((vNoise.r - fRatio) < g_DesolvePower * 0.01f)
        {
            Out.vColor.rgb = g_DesolveColor;
        }

        if (vNoise.r < fRatio)
        {
            discard;
        }
    }
    return Out;
}

PS_OUT PS_BLADE_BLOOM(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    float fRatio = In.vLifeTime.y / In.vLifeTime.x;

    float2 adjustedUV = In.vTexcoord;
    if (fRatio <= 0.2)
    {
        float normalizedRatio = fRatio / 0.2;
        adjustedUV.x = lerp(-1.0, In.vTexcoord.x, normalizedRatio);
    }
    if (adjustedUV.x >= 0.0 && adjustedUV.x <= 1.0)
    {
        Out.vColor = g_Texture.Sample(LinearSampler, adjustedUV);
    }
    else
    {
        // UV가 유효 범위를 벗어나면 투명하게 처리
        Out.vColor = float4(0, 0, 0, 0);
    }

    float4 vNoise = g_DesolveTexture.Sample(LinearSampler, In.vTexcoord);
    if (Out.vColor.a < 0.1f)
        discard;

    Out.vColor.a = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.0f;

    if (g_Alpha)
    {
        Out.vColor.a *= g_BlurPower * (1 - fRatio);
    }
    else
    {
        Out.vColor.a *= g_BlurPower;
    }

    Out.vColor.rgb = g_BloomColor;

    if (g_Desolve)
    {
        if ((vNoise.r - fRatio) < g_DesolvePower * 0.01f)
        {
            Out.vColor.rgb = g_DesolveColor;
        }

        if (vNoise.r < fRatio)
        {
            discard;
        }
    }
    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPREAD();
    }

    pass BloomPass
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLOOM();
    }

    pass BLADE
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DS_Particle, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLADE();
    }

    pass BLADE_BLOOM
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DS_Particle, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLADE_BLOOM();
    }
  
}

