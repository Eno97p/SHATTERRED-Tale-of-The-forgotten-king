#include "Engine_Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_Texture;
texture2D	g_MaskTexture;
texture2D	g_DisolveTexture;
texture2D	g_OpacityTex;

bool		g_bIsFadeIn;
float		g_fAlphaTimer;
float		g_DisolveValue = 1.f;

float		g_fFlowTime;
float		g_CurrentRatio;
float		g_PastRatio;
float		g_HudRatio = 1.f;

// 글리치 시도
texture2D	g_NoiseTexture;
float		g_GlitchTimer;
float2		g_vResolution = float2(1280.f, 820.f); // 임의로

matrix		g_RotationMatrix; // 회전 행렬

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

VS_OUT VS_ROTATION(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	float3 texCoord3 = float3(In.vTexcoord - 0.5, 1.0);
	float3 rotatedTexcoord = mul(texCoord3, g_RotationMatrix);
	Out.vTexcoord = rotatedTexcoord.xy + 0.5;

	return Out;
}

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
	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_FLOW(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 vDetailUV = In.vTexcoord;
	vDetailUV.y -= g_fFlowTime;

	vector		vDestDiffuse = g_Texture.Sample(LinearSampler, vDetailUV);
	vector		vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);

	if (0.1f > vMask.r && 0.1f > vMask.g && 0.1f > vMask.b)
		discard;

	vector		vResult = vDestDiffuse * vMask;

	Out.vColor = vResult;

	if (Out.vColor.a < 0.1f)
		discard;

	if (g_bIsFadeIn)
	{
		Out.vColor.a = 1.f - g_fAlphaTimer;
	}
	else
	{
		Out.vColor.a = g_fAlphaTimer;
	}

	return Out;
}

PS_OUT PS_MAIN_FLOW_HORIZONTAL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 vDetailUV = In.vTexcoord;
	vDetailUV.x -= g_fFlowTime;

	vector		vDestDiffuse = g_Texture.Sample(LinearSampler, vDetailUV);

	Out.vColor = vDestDiffuse;

	if (Out.vColor.a < 0.1f)
		discard;

	if (0.2 < Out.vColor.a)
		Out.vColor.a = 0.2;

	return Out;
}

PS_OUT PS_MAIN_FLOW_HORIZONTAL_SHOPBG(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 vDetailUV = In.vTexcoord;
	vDetailUV.y += g_fFlowTime;

	vector		vDestDiffuse = g_Texture.Sample(LinearSampler, vDetailUV);

	Out.vColor = vDestDiffuse;

	return Out;
}

PS_OUT PS_MAIN_FLOW_VERTICAL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 vDetailUV = In.vTexcoord * 3;
	vDetailUV.y += g_fFlowTime;

	vector		vDestDiffuse = g_Texture.Sample(LinearSampler, vDetailUV);
	vector		vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);

	if (0.8f > vMask.r && 0.8f > vMask.g && 0.8f > vMask.b) // 0.1f > vMask.r && 0.1f > vMask.g && 0.1f > vMask.b
		discard;

	vector		vResult = vDestDiffuse * vMask;

	Out.vColor = vResult;

	if (Out.vColor.a < 0.9f)
		discard;

	if (g_bIsFadeIn)
	{
		Out.vColor.a = 1.f - g_fAlphaTimer;
	}
	else
	{
		float fResultAlpha = g_fAlphaTimer;

		if (0.05 < fResultAlpha)
			fResultAlpha = 0.05;

		Out.vColor.a = fResultAlpha;
	}

	return Out;
}

PS_OUT PS_HUD(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	if (Out.vColor.a < 0.1f)
		discard;
	if (g_PastRatio > g_CurrentRatio)
	{
		if (In.vTexcoord.x > g_PastRatio * g_HudRatio + (1.f - g_HudRatio) * 0.5f) discard;
		else if (In.vTexcoord.x > g_CurrentRatio * g_HudRatio + (1.f - g_HudRatio) * 0.5f) Out.vColor = float4(1.f, 1.f, 0.5f, Out.vColor.a);
	}
	else
	{
		if (In.vTexcoord.x > g_PastRatio * g_HudRatio + (1.f - g_HudRatio) * 0.5f) discard;
	}


	return Out;
}

PS_OUT PS_BUFFTIMER(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	if (Out.vColor.a < 0.1f)
		discard;

	if (In.vTexcoord.x > (1.f - g_CurrentRatio) * g_HudRatio + (1.f - g_HudRatio) * 0.5f)
		discard;

	return Out;
}

PS_OUT PS_FADE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	//if (Out.vColor.a < 0.1f)
	//	discard;

	float fAlpha = Out.vColor.a;

	if (g_bIsFadeIn)
	{
		if (fAlpha < (1.f - g_fAlphaTimer))
			Out.vColor.a = fAlpha;
		else
			Out.vColor.a = 1.f - g_fAlphaTimer;

		//Out.vColor.a = 1.f - g_fAlphaTimer;
	}
	else
	{
		float fResultAlpha = g_fAlphaTimer;

		if (fAlpha < fResultAlpha)
			fResultAlpha = fAlpha;

		Out.vColor.a = fResultAlpha;
	}

	return Out;
}

PS_OUT PS_FADE_DASH(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	if (Out.vColor.a < 0.1f)
		discard;

	float fAlpha = Out.vColor.a;
	fAlpha = 0.8f;

	Out.vColor.r = 0.8f;   //0.1f;
	Out.vColor.g = 0.8f;   // 0.1f;
	Out.vColor.b = 0.8f;   //0.2f;

	if (g_bIsFadeIn)
	{
		if (fAlpha < (1.f - g_fAlphaTimer))
			Out.vColor.a = fAlpha;
		else
			Out.vColor.a = 1.f - g_fAlphaTimer;
	}
	else
	{
		float fResultAlpha = g_fAlphaTimer;

		if (fAlpha < fResultAlpha)
			fResultAlpha = fAlpha;

		Out.vColor.a = fResultAlpha;
	}

	return Out;
}

PS_OUT PS_NOT_FADE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	if (Out.vColor.a < 0.1f)
		discard;

	/*if (0.2 < Out.vColor.a)
		Out.vColor.a = 0.2;*/

	return Out;
}

PS_OUT PS_FADE_DISSOLVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
	if (vDiffuse.a < 0.1f)
		discard;

	vector vDissolve = g_DisolveTexture.Sample(LinearSampler, In.vTexcoord);
	float dissolveValue = (vDissolve.r + vDissolve.g + vDissolve.b) / 3.f;

	if (g_bIsFadeIn)
	{
		if ((g_DisolveValue - dissolveValue) < 0.05f)
		{
			Out.vColor = vDiffuse;
			return Out;
		}
		else if (dissolveValue < g_DisolveValue)
		{
			discard;
		}
		else
		{
			Out.vColor.rgb = float3(0.f, 0.f, 0.f);
			Out.vColor.a = vDiffuse.a;
		}
	}
	else
	{
		if ((g_DisolveValue - dissolveValue) > 0.05f)
		{
			Out.vColor = vDiffuse;
			return Out;
		}
		else if (dissolveValue > g_DisolveValue)
		{
			discard;
		}
		else
		{
			Out.vColor.rgb = float3(0.f, 0.f, 0.f);
			Out.vColor.a = vDiffuse.a;
		}
	}

	return Out;
}

PS_OUT PS_ALPHA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	// >> discard 안 해도 그대로?

	return Out;
}

PS_OUT PS_GLITCH(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vGlitchUV = In.vTexcoord; // 텍스쳐 좌표와 색상 샘플링
	
	float2 vNoiseUV = In.vTexcoord * g_vResolution / 100.f;
	
	vNoiseUV.x -= g_GlitchTimer * 10.f; // g_fGlitchSpeed; // 시간에 따른 노이즈 텍스쳐으 ㅣY축 이동

	float fNoiseValue = g_NoiseTexture.Sample(LinearSampler, vNoiseUV).r; // 노이즈 값 가져오기

	// 글리치 효과 적용을 위한 텍스쳐 좌표 왜곡
	vGlitchUV.x += 10.f * (sin(g_GlitchTimer * 10.0 + fNoiseValue * 10.0) * 0.02f); // g_fGlitchIntensity -> 3.f

	// 글리치 효과를 위한 RGB 채널 분리
	float4 vColorR = g_Texture.Sample(LinearSampler, vGlitchUV + float2(0.005f, 0.0f));
	float4 vColorG = g_Texture.Sample(LinearSampler, vGlitchUV);
	float4 vColorB = g_Texture.Sample(LinearSampler, vGlitchUV - float2(0.005f, 0.0f));

	// RGB 채널을 합쳐 최종 색상 계산
	Out.vColor = float4(vColorR.r, vColorG.g, vColorB.b, 1.0f);



	//Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	float fAlpha = Out.vColor.a;

	if (g_bIsFadeIn)
	{
		if (fAlpha < (1.f - g_fAlphaTimer))
			Out.vColor.a = fAlpha;
		else
			Out.vColor.a = 1.f - g_fAlphaTimer;
	}
	else
	{
		float fResultAlpha = g_fAlphaTimer;

		if (fAlpha < fResultAlpha)
			fResultAlpha = fAlpha;

		Out.vColor.a = fResultAlpha;
	}

	return Out;
}

PS_OUT PS_PORTALPIC(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 uv = In.vTexcoord;

	uv.x += g_fFlowTime * 0.1f;

	vector Color = g_Texture.Sample(LinearSampler, In.vTexcoord);
	vector vOpacity = g_OpacityTex.Sample(LinearSampler, uv);
	vector vDissolve = g_DisolveTexture.Sample(LinearSampler, In.vTexcoord);

	float dissolveValue = (vDissolve.r + vDissolve.g + vDissolve.b) / 3.f;

	if (In.vTexcoord.y <= 0.5) {
		Color.a *= (In.vTexcoord.y) *2.0;
	}

	if (Color.r < vOpacity.r)
		Color.rgb += 0.1f;

	Out.vColor = Color;

	if (!g_bIsFadeIn)
	{
		if ((g_DisolveValue - dissolveValue) < 0.05f)
		{
			Out.vColor = Color;
			return Out;
		}
		else if (dissolveValue < g_DisolveValue)
		{
			discard;
		}
		else
		{
			Out.vColor.rgb = float3(0.f, 0.f, 0.f);
			Out.vColor.a = Color.a;
		}
	}

	return Out;
}

PS_OUT PS_PORTALPIC_Bloom(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;


	float2 uv = In.vTexcoord;

	uv.x += g_fFlowTime * 0.1f;


	vector Color = g_Texture.Sample(LinearSampler, In.vTexcoord);

	vector vOpacity = g_OpacityTex.Sample(LinearSampler, uv);

	if (In.vTexcoord.y <= 0.5) {
		Color.a *= (In.vTexcoord.y) * 2.0;
	}

	if (Color.r < vOpacity.r)
		Color.rgb += 0.1f;

	Color.a *= 0.3f;

	Out.vColor = Color;

	return Out;
}

technique11 DefaultTechnique
{
	pass DefaultPass_0
	{
		SetRasterizerState(RS_NoCull); // RS_NoCull   RS_Default
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass FlowPass_1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FLOW();
	}

	pass HUD_2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_HUD();
	}

	pass FadeInOut_3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FADE();
	}

		pass Fade_Dissolve_4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FADE_DISSOLVE();
	}

		pass RotationPass_5
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_ROTATION();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass FlowHorizontalPass_6
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FLOW_HORIZONTAL();
	}

	pass FlowVerticalPass_7
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FLOW_VERTICAL();
	}

	pass AlphaPass_8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHA();
	}

	pass ShopBG_FlowHorizontalPass_9
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FLOW_HORIZONTAL_SHOPBG();
	}

	pass FlowHorizontalPass_10
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_NOT_FADE();
	}

	pass Fade_Dash_11
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FADE_DASH();
	}

	pass HUD_12
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BUFFTIMER();
	}

	pass Glitch_13
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_GLITCH();
	}

	pass PortalPic_14
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PORTALPIC();
	}

	pass PortalPic_15
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PORTALPIC_Bloom();
	}	
}

