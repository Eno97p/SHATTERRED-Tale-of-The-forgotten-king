#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix		g_ViewMatrixInv, g_ProjMatrixInv;	// Shader에는 역행렬 구해주는 함수가 없으므로 월드, 뷰, 투영 행렬을 가지고 있더라도 역행렬 연산해서 활용 불가능 -> 외부에서 받아와야 함
												//(직교 행렬의 경우 Transpos(전치)로 역행렬 구하기 가능. 단, 위치는 직교의 개념이 없으므로 4x4가 아니라 3x3)
vector		g_vLightDir;
vector		g_vLightPos;
float		g_fLightRange;
vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

vector		g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector		g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector		g_vCamPosition;

texture2D	g_Texture;
texture2D	g_NormalTexture;
texture2D	g_DiffuseTexture;
texture2D	g_ShadeTexture;
texture2D	g_DepthTexture;
texture2D	g_SpecularTexture;

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

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	return Out;
}

struct PS_OUT_LIGHT
{
	vector		vShade : SV_TARGET0;
	vector		vSpecular : SV_TARGET1; // 이거 객체 Specular인가? 빛 연산 맞나?
};

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In) // 방향성 광원 셰이더
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	//Out.vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal)), 0.f);

	Out.vShade = g_vLightDiffuse *
		saturate(max(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));

	// vWorldPos : 화면에 그려지고 있는 픽셀들의 실제 월드 위치
	// 렌더 타겟에 객체들 그릴 때 픽셀의 월드 위치를 저장하는 방법 (월드 공간은 범위가 무제한이라는 조건 때문에 저장 힘듦)
	// 현재 상황에서 픽셀의 투영 위치(x, y)는 구할 수 있음 -> z가 없기 때문에 월드까지의 역변환 난해 -> 투영 z(0~1), 뷰스페이스의 픽셀 z(near~far)를 받아옴(무제한X)

	vector	vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);

	vector vWorldPos;

	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; // vDepthDesc.x : 투영 공간 상의 z가 담겨 있음
	vWorldPos.w = 1.f;

	vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f); // 얘도 far로 해야 하지 않을까 싶긴 함

	vWorldPos = mul(vWorldPos, g_ProjMatrixInv); // 뷰 스페이스 상 위치
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv); // 월드 스페이스 상 위치

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));
	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f);

	return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In) // 점 광원 셰이더
{
	PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;

	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
	vector vWorldPos;

	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
	vWorldPos.w = 1.f;

	vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);

	/* 뷰스페이스 상의 위치를 구한다. */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치를 구한다. */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vLightDir = vWorldPos - g_vLightPos;
	float		fDistance = length(vLightDir);

	float		fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;

	Out.vShade = g_vLightDiffuse *
		saturate(max(dot(normalize(vLightDir) * -1.f, normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;

	vector	vReflect = reflect(normalize(vLightDir), normalize(vNormal));
	vector	vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f) * fAtt;

	return Out;
}

PS_OUT PS_MAIN_DEFERRED_RESULT(PS_IN In) // 후처리 셰이더
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	if (0.f == vDiffuse.a)
		discard;

	vector		vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
	vector		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

	Out.vColor = vDiffuse * vShade + vSpecular * 0.3f; // vSpecular

	return Out;
}

technique11 DefaultTechnique
{
	/* 특정 렌더링을 수행할 때 적용해야할 셰이더 기법의 셋트들의 차이가 있다. */
	pass DebugRender
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass Light_Directional
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_Test_None_Write, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_Test_None_Write, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
	}

	pass Deferred_Result
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_Test_None_Write, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFERRED_RESULT();
	}
}

