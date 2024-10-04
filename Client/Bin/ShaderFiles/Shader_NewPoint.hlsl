#include "Engine_Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_Texture, g_DesolveTexture, g_DepthTexture;
vector		g_vCamPosition;
float		g_BlurPower, g_DesolvePower;
float3		g_StartColor, g_EndColor, g_DesolveColor, g_BloomColor;
bool		g_Desolve, g_Alpha, g_Color;

struct VS_IN
{
	float3      vPosition : POSITION;
	float2      vTexcoord : TEXCOORD0;
	float4      vRight : TEXCOORD1;
	float4      vUp : TEXCOORD2;
	float4      vLook : TEXCOORD3;
	float4      vTranslation : TEXCOORD4;
	float2      vLifeTime : COLOR0;
};

struct VS_OUT
{
	float3		vPosition : POSITION;
	float2		vPSize : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float2		vLifeTime : COLOR0;

};

float FindMaxValue(float a, float b, float c)
{
	// 중첩된 max 함수를 사용하여 세 값 중 가장 큰 값을 찾음
	return max(a, max(b, c));
}

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	float4x4 TransformMatrix = float4x4(
		In.vRight,
		In.vUp,
		In.vLook,
		In.vTranslation
	);

	vector		vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix).xyz;

	float XScale = In.vRight.x;
	float YScale = In.vUp.y;
	float ZScale = In.vLook.z;

	Out.vPSize.x = max(XScale, YScale);
	Out.vPSize.y = Out.vPSize.x;
	//Out.vPSize.x = In.vRight.x;
	//Out.vPSize.y = In.vUp.y;

	Out.vLifeTime = In.vLifeTime;

	Out.vProjPos = float4(Out.vPosition, 1.f);
	return Out;
}

struct GS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float2		vLifeTime : COLOR0;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
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

	Out[0].vLifeTime = In[0].vLifeTime;
	Out[1].vLifeTime = In[0].vLifeTime;
	Out[2].vLifeTime = In[0].vLifeTime;
	Out[3].vLifeTime = In[0].vLifeTime;
	
	vector			vLook = normalize(g_vCamPosition - vector(In[0].vPosition, 1.f));
	float3			vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f;
	float3			vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * 0.5f;

	float3			vPosition;
	matrix			matVP = mul(g_ViewMatrix, g_ProjMatrix);

	vPosition = In[0].vPosition + vRight + vUp;
	Out[0].vProjPos = Out[0].vPosition = mul(float4(vPosition, 1.f), matVP);
	Out[0].vTexcoord = float2(0.f, 0.f);


	vPosition = In[0].vPosition - vRight + vUp;
	Out[1].vProjPos =Out[1].vPosition = mul(float4(vPosition, 1.f), matVP);
	Out[1].vTexcoord = float2(1.f, 0.f);

	vPosition = In[0].vPosition - vRight - vUp;
	Out[2].vProjPos = Out[2].vPosition = mul(float4(vPosition, 1.f), matVP);
	Out[2].vTexcoord = float2(1.f, 1.f);

	vPosition = In[0].vPosition + vRight - vUp;
	Out[3].vProjPos = Out[3].vPosition = mul(float4(vPosition, 1.f), matVP);
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



struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float2		vLifeTime : COLOR0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};


PS_OUT PS_DEFAULT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, In.vTexcoord);

	float2		vDepthCoord;
	vDepthCoord.x = (In.vProjPos.x/ In.vProjPos.w) * 0.5f + 0.5f;
	vDepthCoord.y = (In.vProjPos.y/ In.vProjPos.w) * -0.5f + 0.5f;
	vector			vOldDepthDesc = g_DepthTexture.Sample(PointSampler, vDepthCoord);

	float fRatio = In.vLifeTime.y / In.vLifeTime.x;
	if (g_Alpha)
	{
		if (Out.vColor.a == 0.f)
			discard;
		Out.vColor.a *= 1- fRatio;
	}
	

	if (g_Color)
	{
		Out.vColor.rgb = lerp(g_StartColor, g_EndColor, fRatio);
	}

	if (g_Desolve)
	{
		if ((vNoise.r - fRatio) < g_DesolvePower * 0.01f)
		{
			Out.vColor.rgb = g_DesolveColor;
		}

		if (vNoise.r < fRatio)
		{
			discard;
		}
	}
	Out.vColor.a = Out.vColor.a * saturate(vOldDepthDesc.y * 3000.f - In.vProjPos.w);

	return Out;
}

PS_OUT PS_BLOOM(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, In.vTexcoord);
	float fRatio = In.vLifeTime.y / In.vLifeTime.x;

	if (Out.vColor.a < 0.1f)
		discard;

	if (g_Alpha)
	{
		Out.vColor.a = lerp(g_BlurPower, 0.f, fRatio);
	}
	else
	{
		Out.vColor.a = g_BlurPower;
	}

	Out.vColor.rgb = g_BloomColor;
	if (g_Desolve)
	{
		if ((vNoise.r - fRatio) < g_DesolvePower * 0.01f)
		{
			Out.vColor.rgb = g_DesolveColor;
		}

		if (vNoise.r < fRatio)
		{
			discard;
		}
	}
	return Out;
}

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFAULT();
	}

	pass BloomPass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLOOM();
	}
}

