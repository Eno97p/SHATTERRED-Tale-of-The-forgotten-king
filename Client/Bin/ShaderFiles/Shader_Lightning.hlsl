#include "Engine_Shader_Defines.hlsli"
#include "ShaderFunctor.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_PerlinNoise;
float3      g_Color;
float       g_fTime;



struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float3 vStartpos : STARTPOSITION;
    float3 vEndpos : ENDPOSITION;
    float2 vLifeTime : LIFETIME;
    float fThickness : THICKNESS;
    float fAmplitude : AMPLITUDE;
    int iNumSegments : NUMSEGMENTS;
};

struct VS_OUT
{
    float3 vStartpos : STARTPOSITION;
    float3 vEndpos : ENDPOSITION;
};


VS_OUT VS_MAIN(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.vStartpos = input.vStartpos;
    output.vEndpos = input.vEndpos;

    return output;
}

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR;
};

[maxvertexcount(2)]
void GS_MAIN(point VS_OUT input[1], inout LineStream<GS_OUT> outputStream)
{
    GS_OUT output;

    // Start point
    output.vPosition = mul(float4(input[0].vStartpos, 1.0f), mul(mul(g_WorldMatrix, g_ViewMatrix), g_ProjMatrix));
    output.vColor = float4(g_Color, 1.0f);
    outputStream.Append(output);

    // End point
    output.vPosition = mul(float4(input[0].vEndpos, 1.0f), mul(mul(g_WorldMatrix, g_ViewMatrix), g_ProjMatrix));
    output.vColor = float4(g_Color, 1.0f);
    outputStream.Append(output);
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN input)
{
    PS_OUT output = (PS_OUT)0;

    output.vColor = input.vColor;

    return output;
}


technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}

