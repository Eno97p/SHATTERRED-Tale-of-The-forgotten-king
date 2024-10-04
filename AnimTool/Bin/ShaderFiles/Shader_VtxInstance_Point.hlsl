#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_Texture;
vector		g_vCamPosition;

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
};

/* 정점 셰이더 :  /* 
/* 1. 정점의 위치 변환(월드, 뷰, 투영).*/
/* 2. 정점의 구성정보를 변경한다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix).xyz;
	Out.vPSize = In.vPSize;

	Out.vLifeTime = In.vLifeTime;

	return Out;
}

struct GS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : TEXCOORD0;

	float2		vLifeTime : COLOR0;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;

	float2		vLifeTime : COLOR0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Out[4];
    Out[0] = (GS_OUT) 0;
    Out[1] = (GS_OUT) 0;
    Out[2] = (GS_OUT) 0;
    Out[3] = (GS_OUT) 0;
	
	

	vector			vLook = g_vCamPosition - vector(In[0].vPosition, 1.f);
	float3			vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f;
	float3			vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * 0.5f;

	float3			vPosition;
	matrix			matVP = mul(g_ViewMatrix, g_ProjMatrix);

	vPosition = In[0].vPosition + vRight + vUp;
	Out[0].vPosition = mul(float4(vPosition, 1.f), matVP);
	Out[0].vTexcoord = float2(0.f, 0.f);

	vPosition = In[0].vPosition - vRight + vUp;
	Out[1].vPosition = mul(float4(vPosition, 1.f), matVP);
	Out[1].vTexcoord = float2(1.f, 0.f);

	vPosition = In[0].vPosition - vRight - vUp;
	Out[2].vPosition = mul(float4(vPosition, 1.f), matVP);
	Out[2].vTexcoord = float2(1.f, 1.f);

	vPosition = In[0].vPosition + vRight - vUp;
	Out[3].vPosition = mul(float4(vPosition, 1.f), matVP);
	Out[3].vTexcoord = float2(0.f, 1.f);

	Triangles.Append(Out[0]);
	Triangles.Append(Out[1]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();

	Triangles.Append(Out[0]);
	Triangles.Append(Out[2]);
	Triangles.Append(Out[3]);
	Triangles.RestartStrip();
}


/* TriangleList인 경우 : 정점 세개를 받아서 w나누기를 각 정점에 대해서 수행한다. */
/* 뷰포트(윈도우좌표로) 변환. */
/* 래스터라이즈 : 정점으로 둘러쌓여진 픽셀의 정보를, 정점을 선형보간하여 만든다. -> 픽셀이 만들어졌다!!!!!!!!!!!! */


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

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord);

	if (Out.vColor.a < 0.1f)
		discard;

	Out.vColor.rgb = float3(1.f, 1.f, 1.f);

	return Out;
}

PS_OUT PS_MAIN_SPREAD(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord);

	/*if (Out.vColor.a < 0.1f || 
		In.vLifeTime.y > In.vLifeTime.x)
		discard;*/

	if (Out.vColor.a < 0.1f)
		discard;

	Out.vColor.a = In.vLifeTime.x - In.vLifeTime.y;

	Out.vColor.rgb = float3(1.f, 1.f, 1.f);

	return Out;
}


technique11 DefaultTechnique
{
	
	/* 특정 렌더링을 수행할 때 적용해야할 셰이더 기법의 셋트들의 차이가 있다. */
	pass Loop
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass NonLoop
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SPREAD();
	}
}

