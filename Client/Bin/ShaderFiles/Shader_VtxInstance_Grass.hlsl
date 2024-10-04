#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_SpecularTexture;

texture2D g_NoiseTexture;  // Perlin 노이즈 텍스처


vector		g_vCamPosition;
float3		g_vTopColor, g_vBotColor;

float4 g_vLightDir = { 0.f, -1.f, 0.f, 1.f };


float g_fAccTime;             // 현재 시간

//바람
const float3 g_vWindDirection = normalize(float3(1.0f, 0.0f, 0.5f));  // 예시 방향, 필요에 따라 조정
float g_fGlobalWindFactor;
float g_fWindStrength;     // 바람 세기

cbuffer GrassConstants : register(b1)
{
	float g_fPlaneOffset;        // 평면 간 오프셋
	float g_fPlaneVertOffset;
	float g_fWindStrengthMultiplier; // 바람 세기 승수
	float g_fGrassFrequency;      // 파동 주파수
	float g_fGrassAmplitude;      // 파동 진폭
	float g_fBillboardFactor;    // 빌보드 팩터
	float g_fLODDistance1;       // LOD 거리 1 (가까운 거리)
	float g_fLODDistance2;       // LOD 거리 2 (중간 거리)
	float g_fElasticityFactor;       // LOD 거리 2 (중간 거리)
	int g_iLODPlaneCount1 = 10;       // LOD 1의 평면 수
	int g_iLODPlaneCount2;       // LOD 2의 평면 수
	int g_iLODPlaneCount3;       // LOD 3의 평면 수
}


struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;
	row_major matrix TransformMatrix : WORLD;
	float2		vLifeTime : COLOR0;
};

struct VS_OUT
{
	float3		vPosition : POSITION;
	float2		vPSize : TEXCOORD0;
	float2		vLifeTime : COLOR0;
	float4		vLocalPos : TEXCOORD1;
	row_major matrix TransformMatrix : WORLD;
	float3		vRandomColor : TEXCOORD2;

};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	Out.vPosition = In.vPosition;
	Out.vPSize = In.vPSize;
	Out.vLifeTime = In.vLifeTime;
	Out.vLocalPos = float4(In.vPosition, 1.f);
	Out.TransformMatrix = In.TransformMatrix;

	// 랜덤 색상 생성
	float3 worldPos = In.TransformMatrix._41_42_43;
	float randomValue = frac(sin(dot(worldPos.xy, float2(12.9898, 78.233))) * 43758.5453);
	Out.vRandomColor = float3(1.0 - randomValue * 0.2, 1.0 - randomValue * 0.2, 1.0 - randomValue * 0.2);

	return Out;
}

struct GS_IN
{
	float3 vPosition : POSITION;
	float2 vPSize : TEXCOORD0;
	float2 vLifeTime : COLOR0;
	float4 vLocalPos : TEXCOORD1;
	row_major matrix TransformMatrix : WORLD;
	float3		vRandomColor : TEXCOORD2;

};

struct GS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float2 vLifeTime : COLOR0;
	float4 vNormal : NORMAL;
	float4 vWorldPos : TEXCOORD1;
	float4 vTangent : TANGENT;
	float4 vBinormal : BINORMAL;
	float4 vProjPos : TEXCOORD2;
	float vLocalPosY : TEXCOORD3;
	float3		vRandomColor : TEXCOORD4;


};

float remap(float value, float inputMin, float inputMax, float outputMin, float outputMax)
{
	return outputMin + (value - inputMin) * (outputMax - outputMin) / (inputMax - inputMin);
}

float easeIn(float t, float a)
{
	return pow(t, a);
}

float SampleNoise(float2 uv)
{
	return g_NoiseTexture.SampleLevel(LinearSampler, uv, 0).r * 2.0 - 1.0;
}

// 3x3 회전 행렬 추출 함수
float3x3 ExtractRotationMatrix(matrix m)
{
	float3x3 rotMat;
	rotMat[0] = normalize(m[0].xyz);
	rotMat[1] = normalize(m[1].xyz);
	rotMat[2] = normalize(m[2].xyz);
	return rotMat;
}

// LOD 레벨을 결정하는 함수 수정
int DetermineLODLevel(float distanceToCamera)
{
	if (distanceToCamera < g_fLODDistance1) return 3;
	else if (distanceToCamera < g_fLODDistance2) return 2;
	else return 1;
}

// 평면 수를 결정하는 함수 수정
int GetPlaneCount(int lodLevel)
{
	if (lodLevel == 3) return g_iLODPlaneCount1;
	else if (lodLevel == 2) return g_iLODPlaneCount2;
	else return g_iLODPlaneCount3;
}
// 최대 렌더링 거리 상수 추가
static const float g_fMaxRenderDistance = 1000.0f; // 이 값은 필요에 따라 조정하세요

[maxvertexcount(30)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
	float3 centerPosition = In[0].TransformMatrix._41_42_43;
	float distanceToCamera = length(centerPosition - g_vCamPosition.xyz);
	if (distanceToCamera > 1000.f)
	{
		return;
	}
	int lodLevel = DetermineLODLevel(distanceToCamera);
	int planeCount = GetPlaneCount(lodLevel);
	float3x3 rotationMatrix = ExtractRotationMatrix(In[0].TransformMatrix);

	// 바람 효과 계산 수정
	float2 baseNoiseUV = centerPosition.xz * 0.01; // 노이즈 스케일 증가
	float slowTime = g_fAccTime * 0.1 * g_fElasticityFactor; // 시간 스케일 감소
	float windStrength = SampleNoise(baseNoiseUV + float2(slowTime, slowTime * 0.7)) * g_fWindStrength * g_fGrassAmplitude;
	float3 windVector = g_vWindDirection * g_fGlobalWindFactor * windStrength;

	// 빌보드 계산
	float3 toCamera = normalize(centerPosition - g_vCamPosition.xyz);
	float3 billboardRight = normalize(cross(float3(0, 1, 0), toCamera));
	float3 billboardUp = float3(0, 1, 0);

	matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

	for (int plane = 0; plane < planeCount; ++plane)
	{
		GS_OUT Out[4];
		for (int i = 0; i < 4; ++i)
			Out[i] = (GS_OUT)0;

		// 각 평면의 오프셋 및 회전 계산
		float angle = (plane / float(planeCount - 1)) * 3.14159;

		// SampleNoise를 사용하여 랜덤 오프셋 생성
		float2 noiseUV = centerPosition.xz * 0.1 + float2(cos(angle), sin(angle));
		float2 randomOffset = SampleNoise(noiseUV) * In[0].vPSize.x * 0.2; // 20% 랜덤 오프셋

		float3 planeOffset = float3(cos(angle), 0, sin(angle)) * (In[0].vPSize.x * g_fPlaneOffset) + float3(randomOffset.x, 0, randomOffset.y);

		// 회전 행렬 계산
		float3x3 planeRotation = {
			cos(angle), 0, -sin(angle),
			0, 1, 0,
			sin(angle), 0, cos(angle)
		};

		// 원래의 방향 벡터
		float3 originalRight = normalize(mul(float3(1, 0, 0), planeRotation));
		float3 originalUp = float3(0, 1, 0);

		// 최종 방향 벡터 계산 (각 평면마다)
		float3 finalRight = lerp(originalRight, billboardRight, g_fBillboardFactor);
		float3 finalUp = lerp(originalUp, billboardUp, g_fBillboardFactor);

		// 스케일 적용
		finalRight *= In[0].vPSize.x * 0.5f;
		finalUp *= In[0].vPSize.y * 0.5f;

		for (int i = 0; i < 4; ++i)
		{
			float3 offset = float3(0, 0, 0);
			if (i == 1 || i == 0) // 상단 정점들에만 바람 효과 적용
			{
				float vertexRatio = lerp(0.3, 0.7, i == 1 ? 1.0 : 0.0);
				offset = windVector * vertexRatio;
			}
			float3 vertexOffset = finalRight * (i == 0 || i == 3 ? -1.0 : 1.0) +
				finalUp * (i < 2 ? 1.0 : -1.0);
			float3 newPosition = centerPosition + planeOffset + vertexOffset + offset;

			// sin wave 추가
			float globalWave = sin(g_fAccTime * 1.5 * g_fElasticityFactor + centerPosition.x * 0.05 + centerPosition.z * 0.05) * 0.2;
			newPosition.x += globalWave * lerp(0.2, 1.0, i < 2 ? 1.0 : 0.0);

			// 추가된 grass wave
			float grassWave = sin(g_fAccTime * g_fGrassFrequency + newPosition.x * 0.1 + newPosition.z * 0.1) * g_fGrassAmplitude;
			newPosition.x += grassWave * lerp(0.2, 1.0, i < 2 ? 1.0 : 0.0);

			// 수직 오프셋 적용 (수정된 부분)
			newPosition.y +=/* g_fPlaneVertOffset +*/ dot(finalUp, float3(0, 1, 0)) * In[0].vPSize.y * 0.5f;

			Out[i].vPosition = mul(float4(newPosition, 1.f), matVP);
			Out[i].vTexcoord = float2(i == 0 || i == 3 ? 0.f : 1.f, i < 2 ? 0.f : 1.f);
			Out[i].vWorldPos = float4(newPosition, 1.f);
			Out[i].vLifeTime = In[0].vLifeTime;
			Out[i].vNormal = float4(finalUp, 0);
			Out[i].vRandomColor = In[0].vRandomColor;
		}

		Triangles.Append(Out[0]);
		Triangles.Append(Out[1]);
		Triangles.Append(Out[2]);
		Triangles.RestartStrip();

		Triangles.Append(Out[0]);
		Triangles.Append(Out[2]);
		Triangles.Append(Out[3]);
		Triangles.RestartStrip();
	}
}

//[maxvertexcount(6)]
//void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
//{
//	GS_OUT Out[4];
//	for (int i = 0; i < 4; ++i)
//		Out[i] = (GS_OUT)0;
//
//	float3 worldPosition = In[0].TransformMatrix._41_42_43;
//	float3x3 rotationMatrix = ExtractRotationMatrix(In[0].TransformMatrix);
//
//	// 바람 효과 계산 복원
//	float2 baseNoiseUV = worldPosition.xz * 0.01; // UV 스케일 감소
//	float2 timeOffset = float2(g_fAccTime * 0.05, g_fAccTime * 0.04); // 시간 스케일 감소
//
//	float2 windDirection = float2(0, 0);
//	windDirection += SampleNoise(baseNoiseUV + timeOffset) * 0.5;
//	windDirection += SampleNoise((baseNoiseUV + float2(1.3, 2.7)) * 1.5 + timeOffset * 0.6) * 0.3;
//
//	float windStrength = (SampleNoise(baseNoiseUV * 2.0 + timeOffset) * 0.5 + 0.5) * g_fWindStrength * 0.1; // 바람 강도 감소
//
//	float3 windVector = mul(float3(windDirection.x, 0, windDirection.y) * windStrength, rotationMatrix);
//
//	float3 vRight = mul(float3(1, 0, 0), rotationMatrix) * In[0].vPSize.x * 0.5f;
//	float3 vUp = mul(float3(0, 1, 0), rotationMatrix) * In[0].vPSize.y * 0.5f;
//
//	matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
//
//	for (int i = 0; i < 4; ++i)
//	{
//		float3 offset = float3(0, 0, 0);
//		if (i == 1 || i == 0) // 상단 정점들에만 바람 효과 적용
//		{
//			float vertexRatio = (i == 1) ? 1.0 : 0.7; // 오른쪽 상단에 더 강한 효과
//			offset = windVector * vertexRatio;
//		}
//
//		float3 localPosition = vRight * (i == 0 || i == 3 ? -0.5 : 0.5) +
//			vUp * (i < 2 ? 0.5 : -0.5);
//
//		float3 newPosition = worldPosition + localPosition + offset;
//
//		Out[i].vPosition = mul(float4(newPosition, 1.f), matVP);
//		Out[i].vTexcoord = float2(i == 0 || i == 3 ? 0.f : 1.f, i < 2 ? 0.f : 1.f);
//		Out[i].vWorldPos = float4(newPosition, 1.f);
//		Out[i].vLifeTime = In[0].vLifeTime;
//		Out[i].vNormal = float4(mul(float3(0, 1, 0), rotationMatrix), 0);
//
//	}
//
//	Triangles.Append(Out[0]);
//	Triangles.Append(Out[1]);
//	Triangles.Append(Out[2]);
//	Triangles.RestartStrip();
//
//	Triangles.Append(Out[0]);
//	Triangles.Append(Out[2]);
//	Triangles.Append(Out[3]);
//	Triangles.RestartStrip();
//}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float2 vLifeTime : COLOR0;
	float4 vNormal : NORMAL;
	float4 vWorldPos : TEXCOORD1;
	float4 vTangent : TANGENT;
	float4 vBinormal : BINORMAL;
	float4 vProjPos : TEXCOORD2;
	float vLocalPosY : TEXCOORD3;
	float3 vRandomColor : TEXCOORD4;
};

struct PS_OUT
{
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
	vector		vSpecular : SV_TARGET3;
	vector		vEmissive : SV_TARGET4;
	vector		vRoughness : SV_TARGET5;
};
// 픽셀 셰이더 수정
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	if (vDiffuse.a < 0.1f)
		discard;

	vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

	float3 vNormal;
	/*{
		vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
		vNormal = vNormalDesc.xyz * 2.f - 1.f;
	}*/
	//else
	{
		vNormal = In.vNormal.xyz * 2.f - 1.f;
	}

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	vNormal = mul(vNormal, WorldMatrix);

	float textureY = In.vTexcoord.y;
	float botColorIntensity = smoothstep(0.0, 0.7, textureY);

	// 랜덤 색상을 적용한 새로운 탑/바텀 컬러 계산
	float3 newTopColor = g_vTopColor * In.vRandomColor;
	float3 newBotColor = g_vBotColor * In.vRandomColor;

	Out.vDiffuse = lerp(float4(newTopColor, 1.f), float4(newBotColor, 1.f), botColorIntensity);
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vSpecular = 0.f;
	Out.vRoughness = 0.f;

	return Out;
}

technique11 DefaultTechnique
{

	/* 특정 렌더링을 수행할 때 적용해야할 셰이더 기법의 셋트들의 차이가 있다. */
	pass Grass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}

