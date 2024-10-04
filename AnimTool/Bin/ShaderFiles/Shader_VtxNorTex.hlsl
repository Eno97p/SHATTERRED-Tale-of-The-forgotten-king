
#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	g_BrushTexture;
texture2D	g_MaskTexture; /* 픽셀다 ㅇ디퓨즈 */
texture2D	g_DiffuseTexture[2]; /* 픽셀다 ㅇ디퓨즈 */

vector		g_vBrushPos = float4(30.f, 0.0f, 20.f, 1.f);
float		g_fBrushRange = 5.f;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vWorldPos : TEXCOORD2;
};

/* 정점 셰이더 : */
/* 1. 정점의 위치 변환(월드, 뷰, 투영).*/
/* 2. 정점의 구성정보를 변경한다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);		
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	return Out;
}

/* TriangleList인 경우 : 정점 세개를 받아서 w나누기를 각 정점에 대해서 수행한다. */
/* 뷰포트(윈도우좌표로) 변환. */
/* 래스터라이즈 : 정점으로 둘러쌓여진 픽셀의 정보를, 정점을 선형보간하여 만든다. -> 픽셀이 만들어졌다!!!!!!!!!!!! */


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vWorldPos : TEXCOORD2;
};

struct PS_OUT
{
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;


	vector		vSourDiffuse = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexcoord * 30.f);
	vector		vDestDiffuse = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexcoord * 30.f);
	vector		vBrush = float4(0.0f, 0.f, 0.f, 1.f);

	vector		vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);

	if (g_vBrushPos.x - g_fBrushRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRange && 
		g_vBrushPos.z - g_fBrushRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRange)
	{
		float2		vTexcoord = (float2)0.f;

		/* (g_vBrushPos.x - g_fBrushRange) : 정해진 영역의 왼쪽 x */
		vTexcoord.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
		vTexcoord.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);

		vBrush = g_BrushTexture.Sample(LinearSampler, vTexcoord);
	}

	vector		vMtrlDiffuse = vDestDiffuse * vMask + vSourDiffuse * (1.f - vMask) + vBrush;
	
	// Out.vDiffuse = vector(0.f, 0.f, 0.f, 1.f);
	Out.vDiffuse = vMtrlDiffuse;

	/* -1 -> 0 */
	/* 1 -> 1 */
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	/* In.vProjPos.z = 0 ~ far */
	/* In.vProjPos.w = near ~ far */
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);

	return Out;
}
//
//PS_OUT PS_MAIN_POINT(PS_IN In)
//{
//	PS_OUT			Out = (PS_OUT)0;
//
//	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord * 30.f);
//
//	vector		vLightDir = In.vWorldPos - g_vLightPos;
//
//	float		fDistance = length(vLightDir);
//
//	vector		fShade = min(max(dot(normalize(vLightDir) * -1.f, In.vNormal), 0.f) + 
//		(g_vLightAmbient * g_vMtrlAmbient), 1.f);
//
//	vector		vReflect = reflect(normalize(vLightDir), In.vNormal);
//	vector		vLook = In.vWorldPos - g_vCamPosition;
//
//	float		fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f);
//	
//	float		fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
//
//	Out.vColor = ((g_vLightDiffuse * vMtrlDiffuse) * fShade + 
//		(g_vLightSpecular * g_vMtrlSpecular) * fSpecular) * fAtt;
//
//	return Out;
//}


technique11 DefaultTechnique
{
	/* 특정 렌더링을 수행할 때 적용해야할 셰이더 기법의 셋트들의 차이가 있다. */
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	//pass Light_Point
	//{
	//	/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
	//	VertexShader = compile vs_5_0 VS_MAIN();
	//	GeometryShader = NULL;
	//	HullShader = NULL;
	//	DomainShader = NULL;
	//	PixelShader = compile ps_5_0 PS_MAIN_POINT();
	//}
}

