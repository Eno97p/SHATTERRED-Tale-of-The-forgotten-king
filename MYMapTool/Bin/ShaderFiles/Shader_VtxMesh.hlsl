#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_SpecularTexture;

texture2D g_RoughnessTexture;
texture2D g_MetalicTexture;
texture2D g_EmissiveTexture;
//FOR DISSOLVE
texture2D	g_NoiseTexture;
float		g_fAccTime;


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


bool g_bDiffuse = false;
bool g_bNormal = false;
bool g_bSpecular = false;
bool g_bOpacity = false;
bool g_bEmissive = false;
bool g_bRoughness = false;
bool g_bMetalic = false;

float g_fFlowSpeed = 0.1f;
float g_fTimeDelta;

//for Grass
float4 g_vColorOffset;
float3 g_WindDirection;
float g_WindStrength;
float g_WindFrequency;
float g_WindGustiness;


//바람 시뮬레이션용
float g_fTime;
float3 g_vWindDirection;
float g_fWindStrength;
float3 g_LeafCol;


//for Card
uint g_TextureNum;

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
	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_WorldMatrix));
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;
	Out.vLocalPos = float4(In.vPosition, 1.f);
	Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), g_WorldMatrix));
	Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);


	return Out;
}

VS_OUT VS_LEAF(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	// 로컬 공간에서의 위치를 사용하여 노이즈 계산
	float3 localPos = In.vPosition;

	// 노이즈 계산
	float positionOffset = sin(localPos.x * 0.1 + localPos.z * 0.1) * 0.5 + 0.5;
	float noise1 = sin(g_fTime * 1.5 + localPos.x * 0.05 + localPos.z * 0.05) * 0.05 * positionOffset;
	float noise2 = cos(g_fTime * 2.0 + localPos.z * 0.1) * 0.03;
	float noise3 = sin(g_fTime * 2.5 + localPos.x * 0.1) * 0.02;
	float verticalNoise = sin(g_fTime + localPos.x * 0.02 + localPos.z * 0.02) * 0.01;

	float3 totalNoise = float3(
		noise1 * g_vWindDirection.x + noise3,
		verticalNoise + noise2 * 0.5,
		noise1 * g_vWindDirection.z + noise2
	) * g_fWindStrength;

	// 버텍스 위치에 따른 가중치 (잎의 끝부분이 더 많이 움직이도록)
	float tipInfluence = saturate((localPos.y - min(localPos.y, 0)) * 0.5 + 0.5);

	// 노이즈를 로컬 위치에 적용
	localPos += totalNoise * tipInfluence;

	// 월드 변환 적용
	float4 worldPos = mul(float4(localPos, 1.f), g_WorldMatrix);

	// 뷰 및 투영 변환 적용
	matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix matWVP = mul(matWV, g_ProjMatrix);
	Out.vPosition = mul(float4(localPos, 1.f), matWVP);

	// 나머지 출력 설정
	Out.vNormal = float4(normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix)).xyz, 0.f);
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;
	Out.vLocalPos = float4(localPos, 1.f);
	Out.vTangent = float4(normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz, 0.f);
	Out.vBinormal = float4(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.f);

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

struct GS_GRASS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
	float4 vLocalPos : TEXCOORD2;
	float4 vTangent : TANGENT;
	float4 vBinormal : BINORMAL;
};
VS_IN VS_MAIN_GRASS(VS_IN In)
{
	return In;
}

//GRASS

[maxvertexcount(3)]
void GS_MAIN_GRASS(triangle VS_IN input[3], inout TriangleStream<GS_GRASS_OUT> outStream)
{
	for (int i = 0; i < 3; i++)
	{
		GS_GRASS_OUT output;
		output.vPosition = mul(float4(input[i].vPosition, 1.0f), mul(mul(g_WorldMatrix, g_ViewMatrix), g_ProjMatrix));
		output.vNormal = mul(float4(input[i].vNormal, 0.0f), g_WorldMatrix);
		output.vTexcoord = input[i].vTexcoord;
		output.vProjPos = output.vPosition;
		output.vLocalPos = float4(input[i].vPosition, 1.0f);
		output.vTangent = mul(float4(input[i].vTangent, 0.0f), g_WorldMatrix);
		output.vBinormal = float4(cross(output.vNormal.xyz, output.vTangent.xyz), 0.0f);
		outStream.Append(output);
	}
	outStream.RestartStrip();
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
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);
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

PS_OUT PS_GRASS_ORDINARY(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	if (vDiffuse.a < 0.1f)
		discard;

	vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	float4 grayColor = float4(0.1, 0.1, 0.1, 1.f);
	float localPosY = In.vLocalPos.y;
	float grayIntensity = 1.0 - saturate(abs(localPosY) / 0.5);

	Out.vDiffuse = lerp(vDiffuse, grayColor, grayIntensity);
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);
	Out.vSpecular = vSpecular;
	Out.vRoughness = 0.f;

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
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);
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

	vector vColor = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);

	Out.vColor = vColor;

	return Out;
}

PS_OUT PS_LEAF(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	if (vDiffuse.a < 0.1f || vDiffuse.r < 0.9f)
		discard;

	vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);

	if (g_bDiffuse)
	{
		float3 baseColor = g_LeafCol;
		float2 centeredUV = In.vTexcoord - 0.5;
		float distanceFromCenter = length(centeredUV);
		float gradient = 1.0 - smoothstep(0.0, 0.5, distanceFromCenter);
		float3 brightColor = baseColor * 1.3;
		float3 finalColor = lerp(baseColor, brightColor, gradient);
		float randomness = frac(sin(dot(In.vPosition.xy, float2(12.9898, 78.233))) * 43758.5453);
		finalColor *= (0.9 + 0.2 * randomness);
		Out.vDiffuse = float4(finalColor, 1.f);
	}

	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);
	if (g_bSpecular) Out.vSpecular = vSpecular;

	vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
	vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
	vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
	if (g_bEmissive) Out.vEmissive = vEmissive;
	if (g_bRoughness) Out.vRoughness = vRoughness;
	if (g_bMetalic) Out.vMetalic = vMetalic;

	return Out;
}


PS_OUT_BLOOM PS_LEAF_BLOOM(PS_IN In)
{
	PS_OUT_BLOOM Out = (PS_OUT_BLOOM)0;

	vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	if (vColor.a < 0.1f || vColor.r < 0.9f)
		discard;

	if (g_bDiffuse)
	{
		Out.vColor = float4(g_LeafCol * 0.5f, 1.f);
	}

	return Out;
}

PS_OUT_BLOOM PS_TRUNK_BLOOM(PS_IN In)
{
	PS_OUT_BLOOM Out = (PS_OUT_BLOOM)0;

	vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	if (vColor.a < 0.1f)
		discard;

	if (g_bDiffuse)
	{
		Out.vColor = vColor * 0.6f + 0.1f;
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
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);

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
	pass DefaultPass_0
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

	pass LightDepth_1
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

	pass ColoredDiffuse_2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_GRASS();
		GeometryShader = compile gs_5_0 GS_MAIN_GRASS();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_GRASS_ORDINARY();
	}

		pass Dissolve_3
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

		pass WireFrame_4
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

		pass AlphaBlend_5
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

		pass Bloom_6
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


	 pass Tree_7
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_LEAF();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_LEAF();
	}

	pass LeafBloom_8
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_LEAF();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_LEAF_BLOOM();
	}

	pass TrunkBloom_9
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_TRUNK_BLOOM();
	}

	pass Card_10
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

	pass Moon_11
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_None_Test_None_Write, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MOON();
	}

}

