#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_Texture , g_DesolveTexture, g_ElectricTex , g_OpacityTex, g_NormalTexture;
float		g_Ratio;

//레이져
float		g_CurTime, g_Speed;
float3		g_Color, g_Color2, g_Color3;
float		g_FrameRatio; 

//블룸 파워
float		g_BloomPower;
bool		g_Opacity = false;

float		g_RadialStrength;
float		g_OpacityPower;

//Fresnel
float		g_FresnelPower;

bool		g_Hit; //ForShield


//vector vOpacity = g_OpacityTex.Sample(LinearSampler, Texcoord);
// if(g_Opacity)Diffuse.a *= vOpacity.r;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3      vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float3      vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct VS_OUT_NORMAL
{
	float4		vPosition : SV_POSITION;
	float4      vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4      vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float3 startPos = g_WorldMatrix[3].xyz;

	// 레이저의 방향 (월드 행렬의 Look 방향)
	float3 laserDir = normalize(g_WorldMatrix[2].xyz);

	// 전체 레이저 길이 (Look 방향 벡터의 스케일)
	float totalLength = length(g_WorldMatrix[2].xyz);

	// 정점의 로컬 위치를 월드 위치로 변환
	float3 worldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix).xyz;

	// 시작 지점에서 정점까지의 거리 계산
	float distanceFromStart = dot(worldPos - startPos, laserDir);

	// 거리에 따른 스케일 계산 (0.1에서 시작하여 1.0까지, 5% 거리에서 최대)
	float rawScale = saturate(distanceFromStart / (totalLength * 0.1f));

	// 비선형 스케일 적용 (더 빠르게 커지도록)
	float scale = lerp(0.2, 1.0, pow(rawScale, 0.2));

	// x와 y 좌표에 스케일 적용
	float2 scaledXY = In.vPosition.xy * scale;

	float3 newPosition = float3(scaledXY, In.vPosition.z);

	Out.vPosition = mul(float4(newPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

VS_OUT VS_CYLINDER(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

VS_OUT_NORMAL VS_METEOR(VS_IN In)
{
	VS_OUT_NORMAL		Out = (VS_OUT_NORMAL)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize
	(mul
	(float4
	(In.vNormal.xyz, 0.f),
		g_WorldMatrix));
	Out.vProjPos = Out.vPosition;
	Out.vTexcoord = In.vTexcoord;

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

struct PS_IN_NORMAL
{
	float4		vPosition : SV_POSITION;
	float4      vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4      vProjPos : TEXCOORD1;
};


PS_OUT PS_LazerBase(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 uv = In.vTexcoord;

	uv.y += g_CurTime * g_Speed;

	vector Color = g_Texture.Sample(LinearSampler, uv);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, uv);

	Color.a = Color.r;

	if (Color.a < 0.1f) discard;
	Color.rgb = g_Color;
	
	if (g_Ratio > 0.7f)
	{
		float dissolveThreshold = (g_Ratio - 0.7f) / 0.3f;
		if (vNoise.r < dissolveThreshold)
		{
			discard; // 픽셀 폐기
		}

	}
	Out.vColor = Color;
	return Out;
}

PS_OUT PS_Cylinder(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 uv = In.vTexcoord;

	uv.y -= g_CurTime * g_Speed;

	vector Color = g_Texture.Sample(LinearSampler, uv);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, uv);

	Color.a = 1- Color.r;

	if (Color.a < 0.1f) discard;
	Color.rgb = g_Color;

	if (g_Ratio > 0.7f)
	{
		float dissolveThreshold = (g_Ratio - 0.7f) / 0.3f;
		if (vNoise.r < dissolveThreshold)
		{
			discard; // 픽셀 폐기
		}

	}
	Out.vColor = Color;
	return Out;
}


PS_OUT PS_Distortion(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 uv = In.vTexcoord;
	vector Color = g_Texture.Sample(LinearSampler, uv);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, uv);

	if (Color.a < 0.1f) discard;
	Color.rgb = g_Color;

	if (g_Ratio > 0.7f)
	{
		float dissolveThreshold = (g_Ratio - 0.7f) / 0.3f;
		if (vNoise.r < dissolveThreshold)
		{
			discard; // 픽셀 폐기
		}

	}
	Out.vColor = Color;
	return Out;
}

PS_OUT Ps_Electric(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	int numFrames = 16;
	float frame = g_FrameRatio * numFrames;
	int frameIndex = (int)frame;
	int row = frameIndex / 4;
	int col = frameIndex % 4;
	float frameSize = 1.0 / 4;
	float2 adjustedTex;
	adjustedTex.x = (In.vTexcoord.x * frameSize) + (col * frameSize);
	adjustedTex.y = (In.vTexcoord.y * frameSize) + (row * frameSize);

	vector Color = g_ElectricTex.Sample(LinearSampler, adjustedTex);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, In.vTexcoord);
	if (Color.a < 0.1f) discard;

	Color.rgb = g_Color;

	if (g_Ratio > 0.7f)
	{
		float dissolveThreshold = (g_Ratio - 0.7f) / 0.3f;
		if (vNoise.r < dissolveThreshold)
		{
			discard; // 픽셀 폐기
		}
	}
	Out.vColor = Color;
	return Out;
}

PS_OUT PS_Rain(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 uv = In.vTexcoord;

	uv.y -= g_CurTime * g_Speed;

	vector Color = g_Texture.Sample(LinearSampler, uv);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, uv);

	Color.a = (Color.r + Color.g + Color.b) / 3.0f;

	if (Color.a < 0.1f) discard;
	Color.rgb = g_Color;

	if (g_Ratio > 0.7f)
	{
		float dissolveThreshold = (g_Ratio - 0.7f) / 0.3f;
		if (vNoise.r < dissolveThreshold)
		{
			discard; // 픽셀 폐기
		}

	}
	Out.vColor = Color;
	return Out;
}

PS_OUT PS_TornadoWind(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 Center = float2(0.5f, 0.5f);
	float2 uv = In.vTexcoord;

	uv.x += g_CurTime * g_Speed;
	uv.y += g_CurTime * g_Speed;

	float2 MovedUV = RadialShear(uv, Center, g_RadialStrength, 0.f);
	
	vector Color = g_Texture.Sample(LinearSampler, MovedUV);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, uv);
	vector vOpacity = g_OpacityTex.Sample(LinearSampler, MovedUV);
	
	if (g_Opacity)
	{
		if (vOpacity.r < g_OpacityPower)
			discard;
	}
		
		

	if (Color.a < 0.1f)
		discard;

	float luminance = dot(Color.rgb, float3(0.299, 0.587, 0.114));
	Color.rgb = lerp(g_Color2, g_Color, luminance);

	if (g_Ratio > 0.7f)
	{
		float dissolveThreshold = (g_Ratio - 0.7f) / 0.3f;
		if (vNoise.r < dissolveThreshold)
		{
			discard; // 픽셀 폐기
		}
	}
	Out.vColor = Color;
	return Out;
}

PS_OUT PS_UVYPLUS(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 uv = In.vTexcoord;

	uv.y += g_CurTime * g_Speed;

	vector Color = g_Texture.Sample(LinearSampler, uv);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, uv);

	Color.a = (Color.r + Color.g + Color.b) / 3.0f;

	if (Color.a < 0.1f) discard;
	Color.rgb = g_Color;

	if (g_Ratio > 0.7f)
	{
		float dissolveThreshold = (g_Ratio - 0.7f) / 0.3f;
		if (vNoise.r < dissolveThreshold)
		{
			discard; // 픽셀 폐기
		}

	}
	Out.vColor = Color;
	return Out;
}

PS_OUT PS_Lightning(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 uv = In.vTexcoord;
	uv.x += g_CurTime * g_Speed;

	vector Color = g_Texture.Sample(LinearSampler, uv);

	if (Color.a < 0.1f) discard;
	Color.rgb = g_Color;

	Out.vColor = Color;
	return Out;
}

PS_OUT PS_Lightning_Bloom(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 uv = In.vTexcoord;
	uv.x += g_CurTime * g_Speed;

	vector Color = g_Texture.Sample(LinearSampler, uv);

	if (Color.a < 0.1f) discard;
	Color.rgb = g_Color;
	Color.a = g_BloomPower;

	Out.vColor = Color;
	return Out;
}

PS_OUT PS_Shield(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 uv = In.vTexcoord;
	float2 originalUV = uv;
	if (uv.y < 0.5f)
	{
		uv.y -= g_CurTime * g_Speed;
	}
	if (uv.y >= 0.5f)
	{
		uv = originalUV;
	}

	vector Color = g_Texture.Sample(LinearSampler, uv);

	Color.a = Color.r;
	if (Color.a < 0.1f) discard;
	Color.rgb = g_Color;

	if (g_Ratio > 0.5f)
	{
		Color.a *= 1 - g_Ratio;
	}


	Out.vColor = Color;
	return Out;
}

PS_OUT PS_Shield_Bloom(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 uv = In.vTexcoord;
	float2 originalUV = uv;
	if (uv.y < 0.5f)
	{
		uv.y -= g_CurTime * g_Speed;
	}
	if (uv.y >= 0.5f)
	{
		uv = originalUV;
	}
	vector Color = g_Texture.Sample(LinearSampler, uv);

	Color.a = Color.r;
	if (Color.a < 0.1f) discard;
	Color.rgb = g_Color;
	Color.a *= g_BloomPower;

	if (g_Ratio > 0.5f)
	{
		Color.a *= 1 - g_Ratio;
	}

	Out.vColor = Color;
	return Out;
}


PS_OUT PS_TornadoWind_Bloom(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 Center = float2(0.5f, 0.5f);
	float2 uv = In.vTexcoord;

	uv.x += g_CurTime * g_Speed;
	uv.y += g_CurTime * g_Speed;

	float2 MovedUV = RadialShear(uv, Center, g_RadialStrength, 0.f);

	vector Color = g_Texture.Sample(LinearSampler, MovedUV);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, uv);
	vector vOpacity = g_OpacityTex.Sample(LinearSampler, MovedUV);

	if (g_Opacity)
	{
		if (vOpacity.r < g_OpacityPower)
			discard;
	}
	if (Color.a < 0.1f)
		discard;

	Color.a *= g_BloomPower;

	float luminance = dot(Color.rgb, float3(0.299, 0.587, 0.114));
	Color.rgb = lerp(g_Color2, g_Color, luminance);

	if (g_Ratio > 0.7f)
	{
		float dissolveThreshold = (g_Ratio - 0.7f) / 0.3f;
		if (vNoise.r < dissolveThreshold)
		{
			discard; // 픽셀 폐기
		}
	}
	Out.vColor = Color;
	return Out;
}

PS_OUT PS_TornadoRoot_Bloom(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 Center = float2(0.5f, 0.5f);
	float2 uv = In.vTexcoord;

	uv.x += g_CurTime * g_Speed;
	uv.y += g_CurTime * g_Speed;

	vector Color = g_Texture.Sample(LinearSampler, uv);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, uv);

	Color.a = Color.r;


	if (Color.a < 0.1f)
		discard;

	Color.a *= g_BloomPower;

	if (g_Ratio > 0.7f)
	{
		float dissolveThreshold = (g_Ratio - 0.7f) / 0.3f;
		if (vNoise.r < dissolveThreshold)
		{
			discard; // 픽셀 폐기
		}
	}
	Out.vColor = Color;
	return Out;
}

PS_OUT PS_YUp(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 Center = float2(0.5f, 0.5f);
	float2 uv = In.vTexcoord;

	uv.y += g_CurTime * g_Speed;

	float2 MovedUV = RadialShear(uv, Center, g_RadialStrength, 0.f);

	vector Color = g_Texture.Sample(LinearSampler, MovedUV);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, uv);
	vector vOpacity = g_OpacityTex.Sample(LinearSampler, MovedUV);

	if (g_Opacity)
	{
		if (vOpacity.r < g_OpacityPower)
			discard;
	}

	if (In.vTexcoord.y < 0.5)
	{
		Color.a = In.vTexcoord.y / 0.5;
	}

	float luminance = dot(Color.rgb, float3(0.299, 0.587, 0.114));
	Color.rgb = lerp(g_Color2, g_Color, luminance);

	if (g_Ratio > 0.7f)
	{
		float dissolveThreshold = (g_Ratio - 0.7f) / 0.3f;
		if (vNoise.r < dissolveThreshold)
		{
			discard; // 픽셀 폐기
		}
	}
	Out.vColor = Color;
	return Out;
}

PS_OUT PS_YUp_Bloom(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 Center = float2(0.5f, 0.5f);
	float2 uv = In.vTexcoord;

	uv.y += g_CurTime * g_Speed;

	float2 MovedUV = RadialShear(uv, Center, g_RadialStrength, 0.f);

	vector Color = g_Texture.Sample(LinearSampler, MovedUV);
	vector vNoise = g_DesolveTexture.Sample(LinearSampler, uv);
	vector vOpacity = g_OpacityTex.Sample(LinearSampler, MovedUV);

	if (g_Opacity)
	{
		if (vOpacity.r < g_OpacityPower)
			discard;
	}

	if (In.vTexcoord.y < 0.5)
	{
		Color.a = In.vTexcoord.y / 0.5;
	}

	Color.a *= g_BloomPower;


	float luminance = dot(Color.rgb, float3(0.299, 0.587, 0.114));
	Color.rgb = lerp(g_Color2, g_Color, luminance);

	if (g_Ratio > 0.7f)
	{
		float dissolveThreshold = (g_Ratio - 0.7f) / 0.3f;
		if (vNoise.r < dissolveThreshold)
		{
			discard; // 픽셀 폐기
		}
	}
	Out.vColor = Color;
	return Out;
}


PS_OUT PS_Aspiration(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 adjustedUV = In.vTexcoord;
	vector Color;
	adjustedUV.y = lerp(-1.0, In.vTexcoord.y + 1, g_Ratio);
	if (adjustedUV.y >= 0.0 && adjustedUV.y <= 1.0)
	{
		Color = g_Texture.Sample(LinearSampler, adjustedUV);
		Color.rgb = g_Color;
		Color.a *= 1 - g_Ratio;
	}
	else
	{
		Color = float4(0, 0, 0, 0);
	}


	Out.vColor = Color;
	return Out;
}

PS_OUT PS_Aspiration_Bloom(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 adjustedUV = In.vTexcoord;
	vector Color;
	adjustedUV.y = lerp(-1.0, In.vTexcoord.y + 1, g_Ratio);
	if (adjustedUV.y >= 0.0 && adjustedUV.y <= 1.0)
	{
		Color = g_Texture.Sample(LinearSampler, adjustedUV);
		Color.rgb = g_Color;
		Color.a *= g_BloomPower;
		Color.a *= 1 - g_Ratio;
	}
	else
	{
		Color = float4(0, 0, 0, 0);
	}

	Out.vColor = Color;
	return Out;
}

PS_OUT PS_XMinusToPlus(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 adjustedUV = In.vTexcoord;
	vector Color;
	adjustedUV.x = lerp(In.vTexcoord.x +1, In.vTexcoord.x -1, g_Ratio);
	if (adjustedUV.x >= 0.0 && adjustedUV.x <= 1.0)
	{
		Color = g_Texture.Sample(LinearSampler, adjustedUV);
		Color.a = Color.r;
		if (Color.a < 0.1f)
			discard;
		Color.rgb = g_Color;
	}
	else
	{
		Color = float4(0, 0, 0, 0);
	}

	Out.vColor = Color;
	return Out;
}

PS_OUT PS_XMinusToPlus_Bloom(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 adjustedUV = In.vTexcoord;
	vector Color;
	adjustedUV.x = lerp(In.vTexcoord.x + 1, In.vTexcoord.x - 1, g_Ratio);
	if (adjustedUV.x >= 0.0 && adjustedUV.x <= 1.0)
	{
		Color = g_Texture.Sample(LinearSampler, adjustedUV);
		Color.a = Color.r;
		if (Color.a < 0.1f)
			discard;
		Color.a *= g_BloomPower;
		Color.rgb = g_Color;
	}
	else
	{
		Color = float4(0, 0, 0, 0);
	}

	Out.vColor = Color;
	return Out;
}

PS_OUT PS_AdjustSpiral(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 adjustedUV = In.vTexcoord;
	vector Color;
	adjustedUV.x = lerp(In.vTexcoord.x + 1, In.vTexcoord.x - 1, g_Ratio);
	if (adjustedUV.x >= 0.0 && adjustedUV.x <= 1.0)
	{
		Color = g_Texture.Sample(LinearSampler, adjustedUV);
		Color.a = Color.r;
		if (Color.a < 0.1f)
			discard;
		Color.rgb = g_Color;

		//여기
		float edgeWidth = 0.1;
		float fadeIn = smoothstep(0.0, edgeWidth, adjustedUV.x);
		float fadeOut = smoothstep(1.0, 1.0 - edgeWidth, adjustedUV.x);
		float alphaBlend = lerp(fadeIn, fadeOut, g_Ratio);
		Color.a *= alphaBlend;
	}
	else
	{
		Color = float4(0, 0, 0, 0);
	}

	Out.vColor = Color;
	return Out;
}

PS_OUT PS_AdjustSpiral_Bloom(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 adjustedUV = In.vTexcoord;
	vector Color;
	adjustedUV.x = lerp(In.vTexcoord.x + 1, In.vTexcoord.x - 1, g_Ratio);
	if (adjustedUV.x >= 0.0 && adjustedUV.x <= 1.0)
	{
		Color = g_Texture.Sample(LinearSampler, adjustedUV);
		Color.a = Color.r;
		if (Color.a < 0.1f)
			discard;
		Color.rgb = g_Color;

		//여기
		float edgeWidth = 0.5;
		float fadeIn = smoothstep(0.0, edgeWidth, adjustedUV.x);
		float fadeOut = smoothstep(1.0, 1.0 - edgeWidth, adjustedUV.x);
		float alphaBlend = lerp(fadeIn, fadeOut, g_Ratio);
		Color.a *= alphaBlend;
		Color.a *= g_BloomPower;
	}
	else
	{
		Color = float4(0, 0, 0, 0);
	}

	Out.vColor = Color;
	return Out;
}

PS_OUT PS_LazerCast(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 adjustedUV = In.vTexcoord;
	vector Color;
	adjustedUV.y = lerp(-1.0, In.vTexcoord.y + 1, g_Ratio);
	if (adjustedUV.y >= 0.0 && adjustedUV.y <= 1.0)
	{
		Color = g_Texture.Sample(LinearSampler, adjustedUV);
		if (Color.a < 0.1f)
			discard;
		Color.rgb = g_Color;
	}
	else
	{
		Color = float4(0, 0, 0, 0);
	}


	Out.vColor = Color;
	return Out;
}

PS_OUT PS_LazerCast_Bloom(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 adjustedUV = In.vTexcoord;
	vector Color;
	adjustedUV.y = lerp(-1.0, In.vTexcoord.y + 1, g_Ratio);
	if (adjustedUV.y >= 0.0 && adjustedUV.y <= 1.0)
	{
		Color = g_Texture.Sample(LinearSampler, adjustedUV);
		if (Color.a < 0.1f)
			discard;
		Color.rgb = g_Color;
		Color.a *= g_BloomPower;
	}
	else
	{
		Color = float4(0, 0, 0, 0);
	}

	Out.vColor = Color;
	return Out;
}

PS_OUT PS_Main(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector Color;

	Color = g_Texture.Sample(LinearSampler, In.vTexcoord);
	if (Color.a < 0.1f)
		discard;
	Color.rgb = g_Color;
	Out.vColor = Color;

	return Out;
}

PS_OUT PS_Main_Bloom(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector Color;

	Color = g_Texture.Sample(LinearSampler, In.vTexcoord);
	if (Color.a < 0.1f)
		discard;

	Color.rgb = g_Color;
	Color.a *= g_BloomPower;

	Out.vColor = Color;
	return Out;
}


PS_OUT PS_MeteorWind(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 adjustedUV = In.vTexcoord;
	vector Color;
	adjustedUV.x = lerp(In.vTexcoord.x + 1, In.vTexcoord.x - 1, g_Ratio);
	if (adjustedUV.x >= 0.0 && adjustedUV.x <= 1.0)
	{
		Color = g_Texture.Sample(LinearSampler, adjustedUV);
		Color.a = Color.r;
		if (Color.a < 0.1f)
			discard;

		if (In.vTexcoord.x >= 0.5) {
			Color.a *= (1.0 - (In.vTexcoord.x - 0.5) * 2.0);
		}

		Color.rgb = g_Color;
	}
	else
	{
		Color = float4(0, 0, 0, 0);
	}

	Out.vColor = Color;
	return Out;
}

PS_OUT PS_MeteorWind_Bloom(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 adjustedUV = In.vTexcoord;
	vector Color;
	adjustedUV.x = lerp(In.vTexcoord.x + 1, In.vTexcoord.x - 1, g_Ratio);
	if (adjustedUV.x >= 0.0 && adjustedUV.x <= 1.0)
	{
		Color = g_Texture.Sample(LinearSampler, adjustedUV);
		Color.a = Color.r;
		if (Color.a < 0.1f)
			discard;
		if (In.vTexcoord.x >= 0.5) {
			Color.a *= (1.0 - (In.vTexcoord.x - 0.5) * 2.0);
		}
		Color.a *= g_BloomPower;
		Color.rgb = g_Color;
	}
	else
	{
		Color = float4(0, 0, 0, 0);
	}

	Out.vColor = Color;
	return Out;
}


PS_OUT PS_Main_Desolve(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector Color, Desolve;
	Color = g_Texture.Sample(LinearSampler, In.vTexcoord);
	Desolve = g_DesolveTexture.Sample(LinearSampler, In.vTexcoord);
	if (Color.a < 0.1f)
		discard;
	Color.rgb = g_Color;


	if (g_FrameRatio < g_Ratio)
	{
		float dissolveProgress = (g_Ratio - g_FrameRatio) / (1.0f - g_FrameRatio);
		if (dissolveProgress > Desolve.r)
		{
			discard;
		}
	}
	Out.vColor = Color;
	return Out;
}

PS_OUT PS_Main_Bloom_Desolve(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector Color, Desolve;

	Color = g_Texture.Sample(LinearSampler, In.vTexcoord);
	Desolve = g_DesolveTexture.Sample(LinearSampler, In.vTexcoord);
	if (Color.a < 0.1f)
		discard;

	Color.rgb = g_Color;
	Color.a *= g_BloomPower;
	if (g_FrameRatio < g_Ratio)
	{
		float dissolveProgress = (g_Ratio - g_FrameRatio) / (1.0f - g_FrameRatio);
		if (dissolveProgress > Desolve.r)
		{
			discard;
		}
	}

	Out.vColor = Color;
	return Out;
}


PS_OUT PS_Main_Alpha(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector Color;
	Color = g_Texture.Sample(LinearSampler, In.vTexcoord);
	if (Color.a < 0.1f)
		discard;
	Color.a = Color.r;
	Color.rgb = g_Color;
	Out.vColor = Color;

	return Out;
}

PS_OUT PS_Main_Alpha_Bloom(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector Color;

	Color = g_Texture.Sample(LinearSampler, In.vTexcoord);
	if (Color.a < 0.1f)
		discard;
	Color.a = Color.r;
	Color.rgb = g_Color;
	Color.a *= g_BloomPower;

	Out.vColor = Color;
	return Out;
}

PS_OUT PS_HexaShield(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 Center = float2(0.5f, 0.5f);
	float2 uv = In.vTexcoord;

	uv.x += g_CurTime * g_Speed;
	uv.y += g_CurTime * g_Speed;

	vector Color = g_Texture.Sample(LinearSampler, uv);
	vector vOpacity = g_OpacityTex.Sample(LinearSampler, uv);
	vector Desolve = g_DesolveTexture.Sample(LinearSampler, In.vTexcoord);
	if (!g_Opacity)
	{
		if (vOpacity.r < g_OpacityPower)
			discard;
	}
	else
	{
		if (Desolve.r < g_Ratio)
			discard;
	}

	Color.a = Color.r;
	Color.rgb = g_Color;

	Out.vColor = Color;
	return Out;
}

PS_OUT PS_HexaShield_Bloom(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 Center = float2(0.5f, 0.5f);
	float2 uv = In.vTexcoord;

	uv.x += g_CurTime * g_Speed;
	uv.y += g_CurTime * g_Speed;

	vector Color = g_Texture.Sample(LinearSampler, uv);
	vector vOpacity = g_OpacityTex.Sample(LinearSampler, uv);
	vector Desolve = g_DesolveTexture.Sample(LinearSampler, In.vTexcoord);

	if (!g_Opacity)
	{
		if (vOpacity.r < g_OpacityPower)
			discard;
	}
	else
	{
		if (Desolve.r < g_Ratio)
			discard;
	}
	Color.a = g_BloomPower;
	Color.a *= Color.r;
	if (g_Hit)
	{
		Color.rgb = lerp(g_Color2, g_Color, g_FrameRatio);
	}
	else
	{
		Color.rgb = g_Color;
	}
	
	Out.vColor = Color;
	return Out;
}

PS_OUT PS_HexaHit(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector Color = g_Texture.Sample(LinearSampler, In.vTexcoord);
	vector vOpacity = g_OpacityTex.Sample(LinearSampler, In.vTexcoord);
	if (vOpacity.r < g_Ratio)
		discard;
	
	Out.vColor = Color;
	return Out;
}

PS_OUT PS_BlackHoleRing(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 uv = In.vTexcoord;
	uv.x += g_CurTime * g_Speed;

	vector Color = g_Texture.Sample(LinearSampler, uv);
	Color.a = Color.r;
	if (Color.a < 0.1f) discard;
	Color.rgb = g_Color;

	Out.vColor = Color;
	return Out;
}

PS_OUT PS_BlackHoleRing_Bloom(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 uv = In.vTexcoord;
	uv.x += g_CurTime * g_Speed;

	vector Color = g_Texture.Sample(LinearSampler, uv);


	Color.a = Color.r;
	if (Color.a < 0.1f) discard;
	Color.rgb = g_Color;
	Color.a *= g_BloomPower;

	Out.vColor = Color;
	return Out;
}

PS_OUT PS_AndrasSphere(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 uv = In.vTexcoord;
	uv.x += g_CurTime * g_Speed;

	vector Color = g_Texture.Sample(LinearSampler, uv);

	if (Color.r < g_OpacityPower)
		discard;


	Out.vColor = Color;
	return Out;
}

PS_OUT PS_AndrasSphere_Bloom(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 uv = In.vTexcoord;
	uv.x += g_CurTime * g_Speed;

	vector Color = g_Texture.Sample(LinearSampler, uv);

	if (Color.r < g_OpacityPower)
		discard;

	Color.a = g_BloomPower;

	Out.vColor = Color;
	return Out;
}



technique11 DefaultTechnique
{
	/* 특정 렌더링을 수행할 때 적용해야할 셰이더 기법의 셋트들의 차이가 있다. */
	pass LazerPass	//0
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_LazerBase();
	}

	pass Distortion	
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Distortion();
	}

	pass Cylinder
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Cylinder();
	}

	pass Cylinder_Distortion
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Distortion();
	}

	pass Electirc
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 Ps_Electric();
	}
	
	pass rain
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Rain();
	}

	pass Tornado	//6Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_TornadoWind();
	}

	pass TornadoSpring	//7pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_UVYPLUS();
	}

	pass Lightning	//8pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Lightning();
	}

	pass Lightning_Bloom	//9pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Lightning_Bloom();
	}

	pass Sheild //10pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Shield();
	}

	pass Sheild_Bloom //11pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Shield_Bloom();
	}

	pass Tornado_Bloom	//12Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_TornadoWind_Bloom();
	}

	pass Tornado_Root_Bloom	//13Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_TornadoRoot_Bloom();
	}

	pass FirePillar_Bottom	//14Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_YUp();
	}

	pass FirePillar_Bottom_Bloom	//15Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_YUp_Bloom();
	}

	pass Aspiration	//16Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Aspiration();
	}

	pass Aspiration_Bloom	//17Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Aspiration_Bloom();
	}

	pass SpiralThick	//18Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_XMinusToPlus();
	}
		

	pass SpiralThick_Bloom	//19Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_XMinusToPlus_Bloom();
	}

	pass AdjustSpiral	//20Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_AdjustSpiral();
	}

	pass AdjustSpiral_Bloom	//21Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_AdjustSpiral_Bloom();
	}


	pass Screw //22pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Cylinder();
	}

	pass LazerCast	//23Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_LazerCast();
	}

	pass LazerCast_Bloom	//24Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_LazerCast_Bloom();
	}

	pass DefaultPass	//25Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Main();
	}

	pass DefaultBloomPass	//26Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Main_Bloom();
	}
		
	pass MeteorWind	//27Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MeteorWind();
	}

	pass MeteorWind_bloom	//28Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MeteorWind_Bloom();
	}

	pass Default_Desolve	//29Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Main_Desolve();
	}

	pass Default_Desolve_Bloom	//30Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Main_Bloom_Desolve();
	}

	pass Default_Alpha	//31Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Main_Alpha();
	}

	pass Default_Alpha_Bloom	//32Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Main_Alpha_Bloom();
	}


	pass HexaShield		//33Pass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_HexaShield();
	}

	pass HexaShield_Bloom	//34Pass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_HexaShield_Bloom();
	}

	pass HexaShield_Hit		//35Pass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Particle, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_HexaHit();
	}
		
	pass BlackHoleRing		//36Pass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BlackHoleRing();
	}

	pass BlackHoleRing_Bloom		//37Pass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BlackHoleRing_Bloom();
	}

	pass AndrasSphere		//38Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_AndrasSphere();
	}

	pass AndrasSphere_Bloom		//39Pass
	{
		SetRasterizerState(RS_NoCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_CYLINDER();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_AndrasSphere_Bloom();
	}


		
}

