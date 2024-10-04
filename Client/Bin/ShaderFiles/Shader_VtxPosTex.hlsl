#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_Texture;
texture2D	g_DepthTexture;

float3		g_Color;
float		g_BloomPower;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

/* 정점 셰이더 :  /* 
/* 1. 정점의 위치 변환(월드, 뷰, 투영).*/
/* 2. 정점의 구성정보를 변경한다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);		
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

struct VS_OUT_SOFTEFFECT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

VS_OUT_SOFTEFFECT VS_MAIN_SOFTEFFECT(VS_IN In)
{
	VS_OUT_SOFTEFFECT		Out = (VS_OUT_SOFTEFFECT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;

	return Out;
}

/* TriangleList인 경우 : 정점 세개를 받아서 w나누기를 각 정점에 대해서 수행한다. */
/* 뷰포트(윈도우좌표로) 변환. */
/* 래스터라이즈 : 정점으로 둘러쌓여진 픽셀의 정보를, 정점을 선형보간하여 만든다. -> 픽셀이 만들어졌다!!!!!!!!!!!! */


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	if (Out.vColor.a < 0.1f) discard;
	

	// Out.vColor = tex2D(DefaultSampler, In.vTexcoord);

	return Out;
}

PS_OUT PS_ITEM(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	
	Out.vColor.a = Out.vColor.r;

	if (Out.vColor.a < 0.1f) discard;

	
	Out.vColor.rgb *= float3(1.f, 1.f, 0.7f - sqrt((In.vTexcoord.x - 0.5f) * (In.vTexcoord.x - 0.5f) + (In.vTexcoord.y - 0.5f) * (In.vTexcoord.y - 0.5f)) * 4.f);

	return Out;
}

PS_OUT PS_ITEM_BLOOM(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	Out.vColor.a = Out.vColor.r * 0.5f;

	if (Out.vColor.a < 0.1f) discard;

	Out.vColor.rgb *= float3(1.f, 1.f, 0.7f - sqrt((In.vTexcoord.x - 0.5f) * (In.vTexcoord.x - 0.5f) + (In.vTexcoord.y - 0.5f) * (In.vTexcoord.y - 0.5f)) * 4.f);

	return Out;
}

struct PS_IN_SOFTEFFECT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

PS_OUT PS_MAIN_SOFTEFFECT(PS_IN_SOFTEFFECT In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	/* 화면 전체기준의 현재 픽셀이 그려져야하는 텍스쳐 좌표. */
	float2		vTexcoord;

	vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector			vOldDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);

	Out.vColor.a = Out.vColor.a * saturate(vOldDepthDesc.y * 3000.f - In.vProjPos.w);
	if (Out.vColor.a < 0.1f) discard;

	return Out;
}

PS_OUT PS_SOFTEFFECT_COLORMAP(PS_IN_SOFTEFFECT In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector Color = g_Texture.Sample(LinearSampler, In.vTexcoord);

	/* 화면 전체기준의 현재 픽셀이 그려져야하는 텍스쳐 좌표. */
	float2		vTexcoord;

	vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector			vOldDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);

	Color.a = Color.a * saturate(vOldDepthDesc.y * 3000.f - In.vProjPos.w);
	if (Color.a == 0.f) discard;
	Color.rgb = g_Color;
	Out.vColor = Color;

	return Out;
}

PS_OUT PS_SOFTEFFECT_COLORMAP_BLOOM(PS_IN_SOFTEFFECT In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector Color = g_Texture.Sample(LinearSampler, In.vTexcoord);

	/* 화면 전체기준의 현재 픽셀이 그려져야하는 텍스쳐 좌표. */
	float2		vTexcoord;

	vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector			vOldDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);

	Color.a = Color.a * saturate(vOldDepthDesc.y * 3000.f - In.vProjPos.w);
	if (Color.a == 0.f) discard;
	Color.rgb = g_Color;
	Color.a *= g_BloomPower;
	Out.vColor = Color;

	return Out;
}


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

	pass SoftEffect
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend , float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN_SOFTEFFECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SOFTEFFECT();
	}

	pass Item_2
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ITEM();
	}

	pass Item_Bloom_3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ITEM_BLOOM();
	}


	pass SoftEffect_Color			//Pass4
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend , float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN_SOFTEFFECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SOFTEFFECT_COLORMAP();
	}

	pass SoftEffect_Color_Bloom		//Pass5
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend , float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN_SOFTEFFECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SOFTEFFECT_COLORMAP_BLOOM();
	}

	
}

