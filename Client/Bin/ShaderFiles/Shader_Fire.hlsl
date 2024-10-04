#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix , g_BloomMatrix;
texture2D	g_FireTexture, g_NoiseTexture, g_AlphaTexture, g_DepthTexture , g_BloomTexture;
float		g_fTime;
float3		g_ScrollSpeeds;
float3		g_Scales;


float2 distortion1;
float2 distortion2;
float2 distortion3;
float distortionScale;
float distortionBias;


struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float2		vTexcoord1 : TEXCOORD1;
	float2		vTexcoord2 : TEXCOORD2;
	float2		vTexcoord3 : TEXCOORD3;
	float4		vProjPos : TEXCOORD4;
};


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float2		vTexcoord1 : TEXCOORD1;
	float2		vTexcoord2 : TEXCOORD2;
	float2		vTexcoord3 : TEXCOORD3;
	float4		vProjPos : TEXCOORD4;
};



struct PS_OUT
{
	vector		vColor : SV_TARGET0;
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

	Out.vTexcoord1 = (In.vTexcoord * g_Scales.x);
	Out.vTexcoord1.y = Out.vTexcoord1.y + (g_fTime * g_ScrollSpeeds.x);

	Out.vTexcoord2 = (In.vTexcoord * g_Scales.y);
	Out.vTexcoord2.y = Out.vTexcoord2.y + (g_fTime * g_ScrollSpeeds.y);

	Out.vTexcoord3 = (In.vTexcoord * g_Scales.z);
	Out.vTexcoord3.y = Out.vTexcoord3.y + (g_fTime * g_ScrollSpeeds.z);


	Out.vProjPos = Out.vPosition;

	return Out;
}



PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 noise1;
	float4 noise2;
	float4 noise3;
	float4 finalnoise;
	float perturb;
	float2 noiseCoords;
	float4 firecolor;
	float4 alphacolor;

	float2		vDepthCoord;
	vDepthCoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vDepthCoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;
	vector			vOldDepthDesc = g_DepthTexture.Sample(PointSampler, vDepthCoord);

	noise1 = g_NoiseTexture.Sample(LinearWrap, In.vTexcoord1);
	noise2 = g_NoiseTexture.Sample(LinearWrap, In.vTexcoord2);
	noise3 = g_NoiseTexture.Sample(LinearWrap, In.vTexcoord3);
	
	noise1 = (noise1 - 0.5f) * 2.f;
	noise2 = (noise2 - 0.5f) * 2.f;
	noise3 = (noise3 - 0.5f) * 2.f;

	noise1.xy = noise1.xy * distortion1.xy;
	noise2.xy = noise2.xy * distortion1.xy;
	noise3.xy = noise3.xy * distortion1.xy;

	finalnoise = noise1 + noise2 + noise3;

	perturb = ((1.f - In.vTexcoord.y) * distortionScale) + distortionBias;

	noiseCoords.xy = (finalnoise.xy * perturb) + In.vTexcoord.xy;

	firecolor = g_FireTexture.Sample(LinearClamp, noiseCoords.xy);

	alphacolor = g_AlphaTexture.Sample(LinearClamp, noiseCoords.xy);

	firecolor.a = alphacolor;

	Out.vColor = firecolor;

	Out.vColor.a = Out.vColor.a * saturate(vOldDepthDesc.y * 3000.f - In.vProjPos.w);

	return Out;
}


technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}

