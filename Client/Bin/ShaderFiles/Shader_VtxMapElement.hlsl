#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix, g_PrevWorldMatrix, g_PrevViewMatrix;
texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_SpecularTexture;


texture2D g_RoughnessTexture;
texture2D g_MetalicTexture;
texture2D g_EmissiveTexture;

//FOR DISSOLVE
texture2D	g_NoiseTexture;
float		g_fAccTime;
float		g_fCamFar = 3000.f;

vector		g_vCamPosition;

TextureCube g_ReflectionTexture; // 반사 텍스처 큐브맵

float4		g_vLightDiffuse = float4(0.5f, 0.f, 1.f, 1.f);
float4		g_vLightAmbient = float4(1.f, 1.f, 1.f, 1.f);
float4		g_vLightSpecular = float4(1.f, 1.f, 1.f, 1.f);
float4		g_vLightDir;

float4		g_vMtrlAmbient = float4(0.4f, 0.4f, 0.4f, 1.f); /* 객체당 엠비언트(모든 픽셀이 같은 엠비언트로 연산한다.) */
float4		g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);

unsigned int g_Red;
unsigned int g_Test;


//for Card
uint g_TextureNum;

bool g_bDiffuse = false;
bool g_bNormal = false;
bool g_bSpecular = false;
bool g_bOpacity = false;
bool g_bEmissive = false;
bool g_bRoughness = false;
bool g_bMetalic = false;
bool g_MotionBlur = false;

float g_fFlowSpeed = 0.1f;
float g_fTimeDelta;

//for Grass
float4 g_vColorOffset;
float3 g_WindDirection;
float g_WindStrength;
float g_WindFrequency;
float g_WindGustiness;


struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vLocalPos : TEXCOORD2;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
	float2		vVelocity : TEXCOORD3;
};

/* 정점 셰이더 :  /*
/* 1. 정점의 위치 변환(월드, 뷰, 투영).*/
/* 2. 정점의 구성정보를 변경한다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT      Out = (VS_OUT)0;

	matrix      matWV, matWVP, matPrevWV, matPrevWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	matPrevWV = mul(g_PrevWorldMatrix, g_PrevViewMatrix);
	matPrevWVP = mul(matPrevWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_WorldMatrix));
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;
	Out.vLocalPos = float4(In.vPosition, 1.f);
	Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), g_WorldMatrix));
	Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);

	vector vPrevPos = mul(float4(In.vPosition, 1.f), matPrevWVP);
	Out.vVelocity = (Out.vPosition.xy / Out.vPosition.w) - (vPrevPos.xy / vPrevPos.w);
	Out.vVelocity *= -2.f;
	return Out;
}


struct VS_OUT_LIGHTDEPTH
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

VS_OUT_LIGHTDEPTH VS_MAIN_LIGHTDEPTH(VS_IN In)
{
	VS_OUT_LIGHTDEPTH		Out = (VS_OUT_LIGHTDEPTH)0;

	//float		fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	//matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
	//	g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
	//	g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
	//	g_BoneMatrices[In.vBlendIndices.w] * fWeightW;



	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;

	return Out;

}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vLocalPos : TEXCOORD2;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
	float2		vVelocity : TEXCOORD3;
};

struct PS_OUT
{
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
	vector		vSpecular : SV_TARGET3;
	vector		vEmissive : SV_TARGET4;
	vector		vRoughness : SV_TARGET5;
	vector		vMetalic : SV_TARGET6;
	float2		vVelocity : SV_TARGET7;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	if (vDiffuse.a < 0.1f)
		discard;
	vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

	float3 vNormal;
	if (g_bNormal)
	{
		vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	}
	else
	{
		vNormal = In.vNormal.xyz * 2.f - 1.f;
	}

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);
	if (g_bDiffuse) Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 1.f);
	if (g_bSpecular) Out.vSpecular = vSpecular;

	// 디퓨즈 맵을 사용하여 러프니스 계산
	float intensity = dot(vDiffuse.rgb, float3(0.299, 0.587, 0.114));
	float calculatedRoughness = 1.0 - intensity;

	// 아티스트 조정 가능한 파라미터 (셰이더 상수로 설정 가능)
	float baseRoughness = 0.8;
	float roughnessContrast = 2.0;

	// 최종 러프니스 계산
	float finalRoughness = saturate(baseRoughness + (calculatedRoughness - 0.5) * roughnessContrast);

	vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
	vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);

	if (g_bEmissive) Out.vEmissive = vEmissive;
	if (g_bRoughness) Out.vRoughness = 0.01f/*vector(finalRoughness, finalRoughness, finalRoughness, 1.0)*/;
	if (g_bMetalic) Out.vMetalic = 1.f - vMetalic;

	//if (g_Red == g_Test)
	//{
	//	Out.vDiffuse = vector(1.f, 0.f, 0.f, 1.f);
	//}
	if (g_MotionBlur)
	{
		Out.vVelocity = In.vVelocity;
	}

	return Out;
}




struct PS_IN_LIGHTDEPTH
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT_LIGHTDEPTH
{
	vector		vLightDepth : SV_TARGET0;
};

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN_LIGHTDEPTH In)
{
	PS_OUT_LIGHTDEPTH		Out = (PS_OUT_LIGHTDEPTH)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	if (vDiffuse.a < 0.1f)
		discard;

	Out.vLightDepth = vector(In.vProjPos.w / 3000.f, 0.0f, 0.f, 0.f);

	return Out;
}


PS_OUT PS_DISSOLVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord);
	if (vNoise.r < g_fAccTime)
	{
		discard;
	}

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	if (vDiffuse.a < 0.1f)
		discard;

	vector		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);
	


	Out.vDiffuse = vDiffuse/* * 0.9f*/;
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 1.f);
	Out.vSpecular = g_vMtrlSpecular;

	//Out.vEmissive = 0.5f;
	/*if (g_Red == 0 || g_Red == 29)
	{
		Out.vDiffuse = vector(1.f, 0.f, 0.f, 1.f);
	}*/
	return Out;
}

PS_OUT PS_WIREFRAME(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	if (vDiffuse.a < 0.1f)
		discard;

	Out.vDiffuse = vector(0.f, 0.f, 1.f, 1.f);

	return Out;
}


struct PS_OUT_AB
{
	vector		vColor : SV_TARGET0;

};

PS_OUT_AB PS_ALPHABLEND(PS_IN In)
{
	PS_OUT_AB Out = (PS_OUT_AB)0;
	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	if (vDiffuse.a < 0.1f)
		discard;

	Out.vColor = vDiffuse;
	Out.vColor.a = 0.2f;

	return Out;

}

struct PS_OUT_BLOOM
{
	vector		vColor : SV_TARGET0;

};

PS_OUT_BLOOM PS_BLOOM(PS_IN In)
{
	PS_OUT_BLOOM Out = (PS_OUT_BLOOM)0;


	if (g_bDiffuse)
	{
		vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
		Out.vColor = vColor * 2.f;
	}

	if (g_bEmissive)
	{
		vector vColor = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
		Out.vColor = vColor;
	}

	

	return Out;
}


PS_OUT PS_CARD(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	// 텍스처 좌표 조정
	float2 adjustedTexcoord = In.vTexcoord;

	// g_TextureNum에 따라 텍스처의 다른 부분 선택
	float startY, endY;
	if (g_TextureNum == 0)
	{
		startY = 0.0f;
		endY = 0.37f;
	}
	else if (g_TextureNum == 1)
	{
		startY = 0.37f;
		endY = 0.68f;
	}
	else // g_TextureNum == 2 또는 그 외의 경우
	{
		startY = 0.68f;
		endY = 1.0f;
	}

	// 선택된 범위 내에서 텍스처 좌표 조정
	adjustedTexcoord.y = startY + (endY - startY) * adjustedTexcoord.y;

	vector vColor = g_DiffuseTexture.Sample(PointSampler, adjustedTexcoord);

	if (vColor.a < 0.1f)
		discard;

	if (g_bDiffuse)
	{
		Out.vDiffuse = vColor;
	}

	float3 vNormal;
	vNormal = In.vNormal.xyz * 2.f - 1.f;
	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 1.f);

	return Out;
}


PS_OUT_BLOOM PS_MOON(PS_IN In)
{
	PS_OUT_BLOOM Out = (PS_OUT_BLOOM)0;

	vector vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);

	if (vColor.a < 0.1f)
		discard;

	//	if (g_bDiffuse)
	{
		Out.vColor = vColor;
	}

	float3 vNormal;
	vNormal = In.vNormal.xyz * 2.f - 1.f;
	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	//Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);

	return Out;
}


technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass LightDepth
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_LIGHTDEPTH();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
	}

	pass ColoredDiffuse
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

		pass Dissolve
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVE();
	}

		pass WireFrame
	{
		SetRasterizerState(RS_Wireframe);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WIREFRAME();
	}

		pass AlphaBlend
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHABLEND();

	}

		pass Bloom
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLOOM();
	}
		pass Card_7
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_CARD();
	}

		pass Moon_8
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MOON();
	}

}

