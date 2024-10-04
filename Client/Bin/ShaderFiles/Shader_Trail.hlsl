#include "Engine_Shader_Defines.hlsli"

/* ������Ʈ ���̺�(������̺�) */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float3 g_StartColor, g_EndColor, g_BloomColor;
texture2D	g_Texture, g_DesolveTexture;
bool g_Desolve , g_Alpha;
float g_BloomPower = 0.f;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
	float2		vLifeTime : COLOR0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;

	float2		vLifeTime : COLOR0;
};

/* ���� ���̴� :  /* 
/* 1. ������ ��ġ ��ȯ(����, ��, ����).*/
/* 2. ������ ���������� �����Ѵ�. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		TransformMatrix = matrix(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector		vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vLifeTime = In.vLifeTime;

	return Out;
}

/* TriangleList�� ��� : ���� ������ �޾Ƽ� w�����⸦ �� ������ ���ؼ� �����Ѵ�. */
/* ����Ʈ(��������ǥ��) ��ȯ. */
/* �����Ͷ����� : �������� �ѷ��׿��� �ȼ��� ������, ������ ���������Ͽ� �����. -> �ȼ��� ���������!!!!!!!!!!!! */


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float2		vLifeTime : COLOR0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};


PS_OUT PS_SWORDTRAIL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	//Out.vColor
	float fRatio = In.vLifeTime.y / In.vLifeTime.x;

	float4 Color = g_Texture.Sample(LinearSampler, In.vTexcoord);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, In.vTexcoord);
	if (Color.a == 0.f)
		discard;

	Out.vColor = Color;
	Out.vColor.a *= 1 - fRatio;

	if (g_Desolve)
	{
		if (vNoise.r < fRatio)
		{
			discard;
		}
	}
	Out.vColor.rgb = lerp(g_StartColor, g_EndColor, fRatio);

	return Out;
}

PS_OUT PS_BLOOM(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, In.vTexcoord);
	if (Out.vColor.a == 0.f)
		discard;

	float fRatio = In.vLifeTime.y / In.vLifeTime.x;

	Out.vColor.a = g_BloomPower;
	Out.vColor.a *= 1- fRatio;
	Out.vColor.rgb = g_BloomColor;

	if (g_Desolve)
	{
		if (vNoise.r < fRatio)
		{
			discard;
		}
	}

	return Out;
}


technique11 DefaultTechnique
{
	
	pass SwordTrail
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SWORDTRAIL();
	}

	pass Trail_Bloom
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLOOM();
	}

}

