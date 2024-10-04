#include "Engine_Shader_Defines.hlsli"

/* ������Ʈ ���̺�(������̺�) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix, g_PrevWorldMatrix, g_PrevViewMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_SpecularTexture;
texture2D g_OpacityTexture;
texture2D g_EmissiveTexture;
texture2D g_RoughnessTexture;
texture2D g_MetalicTexture;
texture2D g_DisolveTexture;
float4 g_DisolveColor = float4(0.f, 1.f, 1.f, 1.f);

float4 g_vCamPosition;

bool g_Hit;
float g_Alpha = 1.f;
float g_DisolveValue = 1.f;

bool g_bDiffuse = false;
bool g_bNormal = false;
bool g_bSpecular = false;
bool g_bOpacity = false;
bool g_bEmissive = false;
bool g_bRoughness = false;
bool g_bMetalic = false;
bool g_MotionBlur = false;

float3 EyeColor;
float g_EyeRatio;   //For Andras CutScene
/* �� �޽ÿ��� ������ �ִ� ����. */
matrix		g_BoneMatrices[512];

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float3		vTangent : TANGENT;

	uint4		vBlendIndices : BLENDINDICES;
	float4		vBlendWeights : BLENDWEIGHTS;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float2		vVelocity : TEXCOORD2;
};

/* ���� ���̴� :  /*
/* 1. ������ ��ġ ��ȯ(����, ��, ����).*/
/* 2. ������ ���������� �����Ѵ�. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	float		fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector		vPrevPos = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);

	matrix		matWV, matWVP, matPrevWV, matPrevWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	matPrevWV = mul(g_PrevWorldMatrix, g_PrevViewMatrix);
	matPrevWVP = mul(matPrevWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;

	vPrevPos = mul(vPrevPos, matPrevWVP);
	Out.vVelocity = (Out.vPosition.xy / Out.vPosition.w) - (vPrevPos.xy / vPrevPos.w);
	Out.vVelocity *= -1.f;

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

	float		fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;

	return Out;

}

/* TriangleList�� ��� : ���� ������ �޾Ƽ� w�����⸦ �� ������ ���ؼ� �����Ѵ�. */
/* ����Ʈ(��������ǥ��) ��ȯ. */
/* �����Ͷ����� : �������� �ѷ��׿��� �ȼ��� ������, ������ ���������Ͽ� �����. -> �ȼ��� ���������!!!!!!!!!!!! */


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float2		vVelocity : TEXCOORD2;
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

[earlydepthstencil]
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	vector vOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);
	vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);

	if (g_bOpacity) vDiffuse.a *= vOpacity.r;
	if (vDiffuse.a < 0.1f)
		discard;

	if (g_bDiffuse) Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 0.f);
	if (g_bEmissive) Out.vEmissive = vEmissive;

	if (g_Hit)
	{
		Out.vDiffuse = float4(1.f, 0.f, 0.f, 0.3f);
	}

	vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
	vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
	if (g_bEmissive) Out.vEmissive = vEmissive;
	if (g_bRoughness) Out.vRoughness = vRoughness;
	if (g_bMetalic) Out.vMetalic = vMetalic;
	if (g_MotionBlur)
	{
		Out.vVelocity = In.vVelocity;
	}
	return Out;
}

PS_OUT PS_WHISPERSWORD(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	vector vOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);
	vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);

	if (g_bOpacity) vDiffuse.a *= vOpacity.r;
	if (vDiffuse.a < 0.1f)
		discard;

	if (g_bDiffuse) Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 0.f);
	if (g_bEmissive) Out.vEmissive = vEmissive;

	vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
	vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
	if (g_bEmissive) Out.vEmissive = vEmissive;
	if (g_bRoughness) Out.vRoughness = vRoughness;
	else Out.vRoughness = vector(0.7f, 0.7f, 0.7f, 1.f);
	if (g_bMetalic) Out.vMetalic = vMetalic;
	else Out.vMetalic = vector(1.f, 1.f, 1.f, 1.f);


	if (In.vTexcoord.y > 757.f / 2048.f && In.vTexcoord.x > 435.f / 2048.f && In.vTexcoord.x < 821.f / 2048.f)
	{
		Out.vDiffuse.a = (Out.vDiffuse.r + Out.vDiffuse.g + Out.vDiffuse.b) / 3.f;
		if (Out.vDiffuse.a < 0.25f) discard;
	}

	vector vDisolve = g_DisolveTexture.Sample(LinearSampler, In.vTexcoord);
	float disolveValue = (vDisolve.r + vDisolve.g + vDisolve.b) / 3.f;
	if (g_MotionBlur)
	{
		Out.vVelocity = In.vVelocity;
	}
	if ((g_DisolveValue - disolveValue) > 0.05f)
	{
		return Out;
	}
	else if (disolveValue > g_DisolveValue)
	{
		discard;
	}
	else
	{
		Out.vDiffuse = g_DisolveColor;
	}

	return Out;
}

struct PS_OUT_BLOOM
{
	vector		vColor : SV_TARGET0;

};

PS_OUT_BLOOM PS_WHISPERSWORD_BLOOM(PS_IN In)
{
	PS_OUT_BLOOM Out = (PS_OUT_BLOOM)0;

	vector vColor = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);

	Out.vColor = vColor;

	return Out;
}

struct PS_OUT_BLEND
{
	vector		vDiffuse : SV_TARGET0;
};

PS_OUT_BLEND PS_ALPHA(PS_IN In)
{
	PS_OUT_BLEND Out = (PS_OUT_BLEND)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	vector vOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);
	vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);

	if (g_bOpacity) vDiffuse.a *= vOpacity.r;

	if (g_bDiffuse) Out.vDiffuse = vDiffuse;
	if (g_bEmissive) Out.vDiffuse += vEmissive;

	if (g_Hit)
	{
		Out.vDiffuse = float4(1.f, 0.f, 0.f, 0.3f);
	}

	Out.vDiffuse.a = g_Alpha;
	return Out;
}

PS_OUT_BLEND PS_CLONE(PS_IN In)
{
	PS_OUT_BLEND Out = (PS_OUT_BLEND)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	vector vOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);

	if (g_bOpacity) vDiffuse.a *= vOpacity.r;

	if (g_bDiffuse) Out.vDiffuse = vDiffuse;
	Out.vDiffuse.a = g_Alpha * 0.5f;

	float3 LightDir = float3(0.f, -1.f, 0.f); // ���� ����
	float3 RimColor = float3(0.5f, 0.5f, 1.f); // ������Ʈ ����
	float RimPower = 2.0f; // ������Ʈ ����

	float3 normal = normalize(In.vNormal);
	float NdotL = dot(normal, -LightDir);

	// ������Ʈ ���
	float rim = 1.0f - saturate(NdotL);

	rim = pow(rim, RimPower);

	Out.vDiffuse.rgb = rim * RimColor;
	Out.vDiffuse.a *= rim;
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

struct PS_OUT_COLOR
{
	vector		vColor : SV_TARGET0;
};

PS_OUT_COLOR PS_DISTORTION(PS_IN In)
{
	PS_OUT_COLOR Out = (PS_OUT_COLOR)0;

	vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	if (vColor.a < 0.1f)
		discard;

	if (g_bDiffuse) Out.vColor = vColor;

	if (g_Hit)
	{
		Out.vColor = float4(1.f, 0.f, 0.f, 0.3f);
	}

	return Out;
}

PS_OUT PS_DISOLVE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	vector vOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);
	vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);

	if (g_bOpacity) vDiffuse.a *= vOpacity.r;
	if (vDiffuse.a < 0.1f)
		discard;

	if (g_bDiffuse) Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 0.f);
	if (g_bEmissive) Out.vEmissive = vEmissive;

	if (g_Hit)
	{
		Out.vDiffuse = float4(1.f, 0.f, 0.f, 0.3f);
	}

	vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
	vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
	if (g_bEmissive) Out.vEmissive = vEmissive;
	if (g_bRoughness) Out.vRoughness = vRoughness;
	if (g_bMetalic) Out.vMetalic = vMetalic;

	vector vDisolve = g_DisolveTexture.Sample(LinearSampler, In.vTexcoord);
	float disolveValue = (vDisolve.r + vDisolve.g + vDisolve.b) / 3.f;
	if (g_MotionBlur)
	{
		Out.vVelocity = In.vVelocity;
	}
	if ((g_DisolveValue - disolveValue) > 0.05f)
	{
		return Out;
	}
	else if (disolveValue > g_DisolveValue)
	{
		discard;
	}
	else
	{
		Out.vDiffuse = g_DisolveColor;
	}
	

	return Out;
}

PS_OUT_COLOR PS_BLUR(PS_IN In)
{
	PS_OUT_COLOR Out = (PS_OUT_COLOR)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	vector vOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);
	vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);

	if (g_bOpacity) vDiffuse.a *= vOpacity.r;
	if (vDiffuse.a < 0.1f)
		discard;

	if (g_bDiffuse) Out.vColor = vDiffuse;

	return Out;
}

PS_OUT_COLOR PS_MAIN_REFLECTION(PS_IN In)
{
	PS_OUT_COLOR Out = (PS_OUT_COLOR)0;

	vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	vector vOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);
	vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);

	if (g_bOpacity) vColor.a *= vOpacity.r;
	if (vColor.a < 0.1f)
		discard;

	if (g_bDiffuse) Out.vColor = vColor;

	if (g_Hit)
	{
		Out.vColor = float4(1.f, 0.f, 0.f, 0.3f);
	}

	return Out;
}

PS_OUT_COLOR PS_DISOLVE_REFLECTION(PS_IN In)
{
	PS_OUT_COLOR Out = (PS_OUT_COLOR)0;

	vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	vector vOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);

	if (g_bOpacity) vColor.a *= vOpacity.r;
	if (vColor.a < 0.1f)
		discard;

	if (g_bDiffuse) Out.vColor = vColor;

	if (g_Hit)
	{
		Out.vColor = float4(1.f, 0.f, 0.f, 0.3f);
	}

	vector vDisolve = g_DisolveTexture.Sample(LinearSampler, In.vTexcoord);
	float disolveValue = (vDisolve.r + vDisolve.g + vDisolve.b) / 3.f;
	if ((g_DisolveValue - disolveValue) > 0.05f)
	{
		return Out;
	}
	else if (disolveValue > g_DisolveValue)
	{
		discard;
	}
	else
	{
		Out.vColor.rgb = float3(0.f, 1.f, 1.f);
	}

	return Out;
}

PS_OUT_COLOR PS_WHISPERSWORD_REFLECTION(PS_IN In)
{
	PS_OUT_COLOR Out = (PS_OUT_COLOR)0;

	vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	vector vOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);

	if (g_bOpacity) vColor.a *= vOpacity.r;
	if (vColor.a < 0.1f)
		discard;

	if (g_bDiffuse) Out.vColor = vColor;

	if (In.vTexcoord.y > 757.f / 2048.f && In.vTexcoord.x > 435.f / 2048.f && In.vTexcoord.x < 821.f / 2048.f)
	{
		Out.vColor.a = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.f;
		if (Out.vColor.a < 0.25f) discard;
	}

	vector vDisolve = g_DisolveTexture.Sample(LinearSampler, In.vTexcoord);
	float disolveValue = (vDisolve.r + vDisolve.g + vDisolve.b) / 3.f;
	if ((g_DisolveValue - disolveValue) > 0.05f)
	{
		return Out;
	}
	else if (disolveValue > g_DisolveValue)
	{
		discard;
	}
	else
	{
		Out.vColor = g_DisolveColor;
	}

	return Out;
}

PS_OUT_COLOR PS_CLOAKING(PS_IN In)
{
	PS_OUT_COLOR Out = (PS_OUT_COLOR)0;

	// �� �������� ��ȯ
	float3 positionVS = mul(float4(In.vPosition.xyz, 1.0), g_ViewMatrix).xyz;
	float3 normalVS = normalize(mul(In.vNormal.xyz, (float3x3)g_ViewMatrix));

	// �� ���������� �� ���� (�׻� (0, 0, -1))
	float3 viewDirVS = float3(0, 0, -1);

	// �ܰ��� ���� ���
	float NdotV = dot(normalVS, viewDirVS);
	float edgeStrength = 1.0 - abs(NdotV);

	// ���� ��� �ܰ��� ��ȭ
	float depthEdge = 1.0 - saturate(positionVS.z * 0.1); // 0.1�� ���� ������ ���Դϴ�.

	// ���� �ܰ��� ����
	float finalEdgeStrength = max(edgeStrength, depthEdge);

	// �ܰ��� �Ӱ谪 ����
	float edgeThreshold = 0.6; // �� ���� �����Ͽ� �ܰ����� �β��� ������ �� �ֽ��ϴ�.

	// �ε巯�� �ܰ��� ��ȯ�� ���� smoothstep ���
	float smoothEdge = smoothstep(edgeThreshold - 0.01, edgeThreshold + 0.01, finalEdgeStrength);

	// ī�޶� ���⿡ ���� ó��
	float facingFactor = saturate(NdotV * 0.5 + 0.5);
	smoothEdge *= facingFactor;

	// �ܰ��� ���� ���� (���)
	float4 edgeColor = float4(1, 1, 1, smoothEdge);

	// ���� ���� ���
	Out.vColor = edgeStrength > edgeThreshold ? edgeColor : float4(0, 0, 0, 0);

	return Out;
}


PS_OUT_COLOR PS_ANDRASEYE(PS_IN In)
{
	PS_OUT_COLOR Out = (PS_OUT_COLOR)0;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	if (vDiffuse.r < 0.7f)
	{
		float3 vDefaultColor = vDiffuse.rgb;
		Out.vColor.rgb = lerp(vDefaultColor, EyeColor, g_EyeRatio);
	}
 
	Out.vColor = vDiffuse;

	return Out;
}


PS_OUT_COLOR PS_ANDRAS_BLOOM(PS_IN In)
{
	PS_OUT_COLOR Out = (PS_OUT_COLOR)0;
	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	if (vDiffuse.r > 0.7f)
		discard;

	Out.vColor.rgb = lerp(vDiffuse.rgb, EyeColor, g_EyeRatio);
	Out.vColor.a = lerp(0.f, 2.f, g_EyeRatio);
	Out.vColor *= 10.f;
	return Out;
}

PS_OUT_COLOR PS_BOSSDECO_BLOOM(PS_IN In)
{
	PS_OUT_COLOR Out = (PS_OUT_COLOR)0;

	Out.vColor = (143.f/ 255.f, 0.9f, 1.f, 1.f);

	return Out;
}


technique11 DefaultTechnique
{
	/* Ư�� �������� ������ �� �����ؾ��� ���̴� ����� ��Ʈ���� ���̰� �ִ�. */
	pass DefaultPass_0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
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

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN_LIGHTDEPTH();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
	}

	pass Alpha_2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHA();
	}

	pass WhisperSword_3
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WHISPERSWORD();
	}

	pass Distortion_4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION();
	}

	pass Clone_5
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_CLONE();
	}

	pass WhisperSword_6
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WHISPERSWORD_BLOOM();
	}

	pass Disolve_7
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISOLVE();
	}

	pass Blur_8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLUR();
	}

	pass Reflection_9
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_None_Test_None_Write, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_REFLECTION();
	}

	pass Reflection_Disolve_10
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_None_Test_None_Write, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISOLVE_REFLECTION();
	}

	pass Reflection_WhisperSword_11
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_None_Test_None_Write, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WHISPERSWORD_REFLECTION();
	}

	pass Cloaking_12
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_CLOAKING();
	}


	pass AndrasEyePass_13
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ANDRASEYE();
	}


	pass AndrasEyePass_Bloom_14
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ANDRAS_BLOOM();
	}

		pass BossDeco_Bloom_15
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BOSSDECO_BLOOM();
	}

		
}