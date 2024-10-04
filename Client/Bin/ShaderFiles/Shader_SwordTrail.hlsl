#include "Engine_Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_Texture, g_DesolveTexture;
float3		g_DiffuseColor;
float		g_BloomPower;
struct VS_IN
{
	float3 Position0 : POSITION0;
	float3 Position1 : POSITION1;
	float3 Position2 : POSITION2;
	float3 Position3 : POSITION3;

	float2 TexCoord0 : TEXCOORD0;
	float2 TexCoord1 : TEXCOORD1;
	float2 TexCoord2 : TEXCOORD2;
	float2 TexCoord3 : TEXCOORD3;

	float2 Lifetime : COLOR0;
};

struct VS_OUT
{
	float3 Position0 : POSITION0;
	float3 Position1 : POSITION1;
	float3 Position2 : POSITION2;
	float3 Position3 : POSITION3;

	float2 TexCoord0 : TEXCOORD0;
	float2 TexCoord1 : TEXCOORD1;
	float2 TexCoord2 : TEXCOORD2;
	float2 TexCoord3 : TEXCOORD3;

	float2 Lifetime : COLOR0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT      Out = (VS_OUT)0;

	vector      Position0 = float4(In.Position0, 1.f);
	vector      Position1 = float4(In.Position1, 1.f);
	vector      Position2 = float4(In.Position2, 1.f);
	vector      Position3 = float4(In.Position3, 1.f);

	matrix		matWV, matWVP, matVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out.Position0 = mul(Position0, g_WorldMatrix).xyz;
	Out.Position1 = mul(Position1, g_WorldMatrix).xyz;
	Out.Position2 = mul(Position2, g_WorldMatrix).xyz;
	Out.Position3 = mul(Position3, g_WorldMatrix).xyz;

	Out.Lifetime = In.Lifetime;
	Out.TexCoord0 = In.TexCoord0;
	Out.TexCoord1 = In.TexCoord1;
	Out.TexCoord2 = In.TexCoord2;
	Out.TexCoord3 = In.TexCoord3;

	return Out;
}



struct GS_IN
{
	float3 Position0 : POSITION0;
	float3 Position1 : POSITION1;
	float3 Position2 : POSITION2;
	float3 Position3 : POSITION3;

	float2 TexCoord0 : TEXCOORD0;
	float2 TexCoord1 : TEXCOORD1;
	float2 TexCoord2 : TEXCOORD2;
	float2 TexCoord3 : TEXCOORD3;

	float2 Lifetime : COLOR0;
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


	float3			vPosition;
	matrix			matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = mul(float4(In[0].Position0, 1.f), matVP);
	Out[0].vTexcoord = In[0].TexCoord0;
	Out[0].vLifeTime = In[0].Lifetime;

	
	Out[1].vPosition = mul(float4(In[0].Position1, 1.f), matVP);
	Out[1].vTexcoord = In[0].TexCoord1;
	Out[1].vLifeTime = In[0].Lifetime;
	
	Out[2].vPosition = mul(float4(In[0].Position2, 1.f), matVP);
	Out[2].vTexcoord = In[0].TexCoord2;
	Out[2].vLifeTime = In[0].Lifetime;
	
	Out[3].vPosition = mul(float4(In[0].Position3, 1.f), matVP);
	Out[3].vTexcoord = In[0].TexCoord3;
	Out[3].vLifeTime = In[0].Lifetime;



	Triangles.Append(Out[0]);
	Triangles.Append(Out[3]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();

	Triangles.Append(Out[0]);
	Triangles.Append(Out[2]);
	Triangles.Append(Out[1]);
	Triangles.RestartStrip();


}



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


PS_OUT PS_DEFAULT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, In.vTexcoord);
	float fRatio = In.vLifeTime.y / In.vLifeTime.x;
	if (vNoise.r < fRatio)
	{
		discard;
	}
	
	Out.vColor.a *= 1 - Out.vColor.r;
	//Out.vColor.a *= 1.3f;
	Out.vColor.rgb = g_DiffuseColor;

	if (Out.vColor.a < 0.2f)
		discard;

	Out.vColor.a *= 1 - fRatio;

	return Out;
}

PS_OUT PS_BLOOM(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, In.vTexcoord);
	float fRatio = In.vLifeTime.y / In.vLifeTime.x;
	if (vNoise.r < fRatio)
	{
		discard;
	}

	Out.vColor.a *= 1 - Out.vColor.r;
	Out.vColor.rgb = g_DiffuseColor;

	if (Out.vColor.a < 0.1f)
		discard;

	Out.vColor.a *= g_BloomPower;

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

