cbuffer cbSettings
{
	static const float g_fWeight[13] =
	{
	   0.0044, 0.0175, 0.0540, 0.1295, 0.2420, 0.3521, 0.3989, 0.3521, 0.2420, 0.1295, 0.0540, 0.0175, 0.0044
	};

	static const float g_fWeight2[52] =
	{
		0.0001, 0.0003, 0.0007, 0.0012, 0.0020, 0.0031, 0.0044, 0.0062, 0.0084, 0.0111, 0.0143, 0.0175,
		0.0220, 0.0271, 0.0329, 0.0396, 0.0471, 0.0540, 0.0630, 0.0731, 0.0844, 0.0969, 0.1104, 0.1295,
		0.2420, 0.3521, 0.3989, 0.3521, 0.2420, 0.1295, 0.1104, 0.0969, 0.0844, 0.0731, 0.0630, 0.0540,
		0.0471, 0.0396, 0.0329, 0.0271, 0.0220, 0.0175, 0.0143, 0.0111, 0.0084, 0.0062, 0.0044, 0.0031,
		0.0020, 0.0012, 0.0007, 0.0003
	};

	static const float g_fWeight3[261] =
	{
	   0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0002, 0.0002,
	   0.0003, 0.0003, 0.0004, 0.0005, 0.0006, 0.0007, 0.0008, 0.0009, 0.0011, 0.0012,
	   0.0014, 0.0016, 0.0018, 0.0021, 0.0023, 0.0026, 0.0029, 0.0033, 0.0037, 0.0041,
	   0.0045, 0.0050, 0.0055, 0.0061, 0.0067, 0.0073, 0.0080, 0.0087, 0.0095, 0.0103,
	   0.0112, 0.0121, 0.0131, 0.0142, 0.0153, 0.0165, 0.0178, 0.0191, 0.0205, 0.0220,
	   0.0236, 0.0252, 0.0269, 0.0287, 0.0306, 0.0326, 0.0347, 0.0368, 0.0391, 0.0414,
	   0.0439, 0.0464, 0.0491, 0.0518, 0.0547, 0.0576, 0.0607, 0.0638, 0.0671, 0.0704,
	   0.0739, 0.0774, 0.0811, 0.0849, 0.0887, 0.0927, 0.0968, 0.1009, 0.1052, 0.1095,
	   0.1140, 0.1185, 0.1231, 0.1278, 0.1326, 0.1375, 0.1424, 0.1475, 0.1526, 0.1578,
	   0.1631, 0.1684, 0.1738, 0.1793, 0.1848, 0.1904, 0.1960, 0.2017, 0.2074, 0.2132,
	   0.2190, 0.2249, 0.2308, 0.2367, 0.2426, 0.2486, 0.2546, 0.2606, 0.2666, 0.2726,
	   0.2786, 0.2846, 0.2906, 0.2965, 0.3025, 0.3084, 0.3143, 0.3201, 0.3259, 0.3316,
	   0.3373, 0.3429, 0.3485, 0.3540, 0.3594, 0.3647, 0.3699, 0.3751, 0.3801, 0.3851,
	   0.3899, 0.3947, 0.3989, 0.3989, 0.3947, 0.3899, 0.3851, 0.3801, 0.3751, 0.3699,
	   0.3647, 0.3594, 0.3540, 0.3485, 0.3429, 0.3373, 0.3316, 0.3259, 0.3201, 0.3143,
	   0.3084, 0.3025, 0.2965, 0.2906, 0.2846, 0.2786, 0.2726, 0.2666, 0.2606, 0.2546,
	   0.2486, 0.2426, 0.2367, 0.2308, 0.2249, 0.2190, 0.2132, 0.2074, 0.2017, 0.1960,
	   0.1904, 0.1848, 0.1793, 0.1738, 0.1684, 0.1631, 0.1578, 0.1526, 0.1475, 0.1424,
	   0.1375, 0.1326, 0.1278, 0.1231, 0.1185, 0.1140, 0.1095, 0.1052, 0.1009, 0.0968,
	   0.0927, 0.0887, 0.0849, 0.0811, 0.0774, 0.0739, 0.0704, 0.0671, 0.0638, 0.0607,
	   0.0576, 0.0547, 0.0518, 0.0491, 0.0464, 0.0439, 0.0414, 0.0391, 0.0368, 0.0347,
	   0.0326, 0.0306, 0.0287, 0.0269, 0.0252, 0.0236, 0.0220, 0.0205, 0.0191, 0.0178,
	   0.0165, 0.0153, 0.0142, 0.0131, 0.0121, 0.0112, 0.0103, 0.0095, 0.0087, 0.0080,
	   0.0073, 0.0067, 0.0061, 0.0055, 0.0050, 0.0045, 0.0041, 0.0037, 0.0033, 0.0029,
	   0.0026, 0.0023, 0.0021, 0.0018, 0.0016, 0.0014, 0.0012, 0.0011, 0.0009, 0.0008,
	   0.0007, 0.0006, 0.0005, 0.0004, 0.0003, 0.0003, 0.0002, 0.0002, 0.0001, 0.0001,
	   0.0001
	};
};

cbuffer cbFixed
{
	static const int gBlurRadius = 5;
	static const int gBlurRadius2 = 25;
	static const int gBlurRadius3 = 130;
};

// ComputeShader.hlsl
Texture2D<float4> gInput : register(t0);
RWTexture2D<float4> gOutput : register(u0);

#define N 256
#define CacheSize (N + 2*gBlurRadius)
#define CacheSize2 (N + 2*gBlurRadius2)
#define CacheSize3 (N + 2*gBlurRadius3)
groupshared float4 gCache[CacheSize];
groupshared float4 gCache2[CacheSize2];
groupshared float4 gCache3[CacheSize3];

[numthreads(N, 1, 1)]
void BlurX1(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID,
	int3 groupID : SV_GroupID)
{
	int globalX = groupThreadID.x + groupID.x * N;

	if (groupThreadID.x < gBlurRadius)
	{
		int x = max(globalX - gBlurRadius, 0);
		gCache[groupThreadID.x] = gInput[int2(x, dispatchThreadID.y)];
	}
	if (groupThreadID.x >= N - gBlurRadius)
	{
		int x = min(globalX + gBlurRadius, gInput.Length.x - 1);
		gCache[groupThreadID.x + 2 * gBlurRadius] = gInput[int2(x, dispatchThreadID.y)];
	}

	gCache[groupThreadID.x + gBlurRadius] = gInput[min(int2(globalX, dispatchThreadID.y), gInput.Length.xy - 1)];

	GroupMemoryBarrierWithGroupSync();

	float4 blurColor = float4(0, 0, 0, 0);

	[unroll]
	for (int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		int k = groupThreadID.x + gBlurRadius + i;
		blurColor += g_fWeight[i + gBlurRadius] * gCache[k];
	}

	gOutput[int2(globalX, dispatchThreadID.y)] = blurColor / 8.f;
}

[numthreads(1, N, 1)]
void BlurY1(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID,
	int3 groupID : SV_GroupID)
{
	int globalY = groupThreadID.y + groupID.y * N;

	if (groupThreadID.y < gBlurRadius)
	{
		int y = max(globalY - gBlurRadius, 0);
		gCache[groupThreadID.y] = gInput[int2(dispatchThreadID.x, y)];
	}
	if (groupThreadID.y >= N - gBlurRadius)
	{
		int y = min(globalY + gBlurRadius, gInput.Length.y - 1);
		gCache[groupThreadID.y + 2 * gBlurRadius] = gInput[int2(dispatchThreadID.x, y)];
	}

	gCache[groupThreadID.y + gBlurRadius] = gInput[min(int2(dispatchThreadID.x, globalY), gInput.Length.xy - 1)];

	GroupMemoryBarrierWithGroupSync();

	float4 blurColor = float4(0, 0, 0, 0);

	[unroll]
	for (int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		int k = groupThreadID.y + gBlurRadius + i;
		blurColor += g_fWeight[i + gBlurRadius] * gCache[k];
	}

	gOutput[int2(dispatchThreadID.x, globalY)] = blurColor / 8.f;
}

[numthreads(N, 1, 1)]
void BlurX2(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID,
	int3 groupID : SV_GroupID)
{
	int globalX = groupThreadID.x + groupID.x * N;

	if (groupThreadID.x < gBlurRadius2)
	{
		int x = max(globalX - gBlurRadius2, 0);
		gCache2[groupThreadID.x] = gInput[int2(x, dispatchThreadID.y)];
	}
	if (groupThreadID.x >= N - gBlurRadius2)
	{
		int x = min(globalX + gBlurRadius2, gInput.Length.x - 1);
		gCache2[groupThreadID.x + 2 * gBlurRadius2] = gInput[int2(x, dispatchThreadID.y)];
	}

	gCache2[groupThreadID.x + gBlurRadius2] = gInput[min(int2(globalX, dispatchThreadID.y), gInput.Length.xy - 1)];

	GroupMemoryBarrierWithGroupSync();

	float4 blurColor = float4(0, 0, 0, 0);

	for (int i = -gBlurRadius2; i <= gBlurRadius2; ++i)
	{
		int k = groupThreadID.x + gBlurRadius2 + i;
		blurColor += g_fWeight2[i + gBlurRadius2] * gCache2[k];
	}

	gOutput[int2(globalX, dispatchThreadID.y)] = blurColor / 8.f;
}

[numthreads(1, N, 1)]
void BlurY2(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID,
	int3 groupID : SV_GroupID)
{
	int globalY = groupThreadID.y + groupID.y * N;

	if (groupThreadID.y < gBlurRadius2)
	{
		int y = max(globalY - gBlurRadius2, 0);
		gCache2[groupThreadID.y] = gInput[int2(dispatchThreadID.x, y)];
	}
	if (groupThreadID.y >= N - gBlurRadius2)
	{
		int y = min(globalY + gBlurRadius2, gInput.Length.y - 1);
		gCache2[groupThreadID.y + 2 * gBlurRadius2] = gInput[int2(dispatchThreadID.x, y)];
	}

	gCache2[groupThreadID.y + gBlurRadius2] = gInput[min(int2(dispatchThreadID.x, globalY), gInput.Length.xy - 1)];

	GroupMemoryBarrierWithGroupSync();

	float4 blurColor = float4(0, 0, 0, 0);

	for (int i = -gBlurRadius2; i <= gBlurRadius2; ++i)
	{
		int k = groupThreadID.y + gBlurRadius2 + i;
		blurColor += g_fWeight2[i + gBlurRadius2] * gCache2[k];
	}

	gOutput[int2(dispatchThreadID.x, globalY)] = blurColor / 8.f;
}

[numthreads(N, 1, 1)]
void BlurX3(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID,
	int3 groupID : SV_GroupID)
{
	int globalX = groupThreadID.x + groupID.x * N;

	if (groupThreadID.x < gBlurRadius3)
	{
		int x = max(globalX - gBlurRadius3, 0);
		gCache3[groupThreadID.x] = gInput[int2(x, dispatchThreadID.y)];
	}
	if (groupThreadID.x >= N - gBlurRadius3)
	{
		int x = min(globalX + gBlurRadius3, gInput.Length.x - 1);
		gCache3[groupThreadID.x + 2 * gBlurRadius3] = gInput[int2(x, dispatchThreadID.y)];
	}

	gCache3[groupThreadID.x + gBlurRadius3] = gInput[min(int2(globalX, dispatchThreadID.y), gInput.Length.xy - 1)];

	GroupMemoryBarrierWithGroupSync();

	float4 blurColor = float4(0, 0, 0, 0);

	for (int i = -gBlurRadius3; i <= gBlurRadius3; ++i)
	{
		int k = groupThreadID.x + gBlurRadius3 + i;
		blurColor += g_fWeight3[i + gBlurRadius3] * gCache3[k];
	}

	gOutput[int2(globalX, dispatchThreadID.y)] = blurColor / 8.f;
}

[numthreads(1, N, 1)]
void BlurY3(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID,
	int3 groupID : SV_GroupID)
{
	int globalY = groupThreadID.y + groupID.y * N;

	if (groupThreadID.y < gBlurRadius3)
	{
		int y = max(globalY - gBlurRadius3, 0);
		gCache3[groupThreadID.y] = gInput[int2(dispatchThreadID.x, y)];
	}

	if (groupThreadID.y >= N - gBlurRadius3)
	{
		int y = min(globalY + gBlurRadius3, gInput.Length.y - 1);
		gCache3[groupThreadID.y + 2 * gBlurRadius3] = gInput[int2(dispatchThreadID.x, y)];
	}

	gCache3[groupThreadID.y + gBlurRadius3] = gInput[min(int2(dispatchThreadID.x, globalY), gInput.Length.xy - 1)];

	GroupMemoryBarrierWithGroupSync();

	float4 blurColor = float4(0, 0, 0, 0);

	for (int i = -gBlurRadius3; i <= gBlurRadius3; ++i)
	{
		int k = groupThreadID.y + gBlurRadius3 + i;
		blurColor += g_fWeight3[i + gBlurRadius3] * gCache3[k];
	}

	gOutput[int2(dispatchThreadID.x, globalY)] = blurColor / 8.f;
}

technique11 Blur
{
	pass BlurX1
	{
		SetVertexShader(NULL);
		SetPixelShader(NULL);
		SetComputeShader(CompileShader(cs_5_0, BlurX1()));
	}

	pass BlurY1
	{
		SetVertexShader(NULL);
		SetPixelShader(NULL);
		SetComputeShader(CompileShader(cs_5_0, BlurY1()));
	}

	pass BlurX2
	{
		SetVertexShader(NULL);
		SetPixelShader(NULL);
		SetComputeShader(CompileShader(cs_5_0, BlurX2()));
	}

	pass BlurY2
	{
		SetVertexShader(NULL);
		SetPixelShader(NULL);
		SetComputeShader(CompileShader(cs_5_0, BlurY2()));
	}

	pass BlurX3
	{
		SetVertexShader(NULL);
		SetPixelShader(NULL);
		SetComputeShader(CompileShader(cs_5_0, BlurX3()));
	}

	pass BlurY3
	{
		SetVertexShader(NULL);
		SetPixelShader(NULL);
		SetComputeShader(CompileShader(cs_5_0, BlurY3()));
	}
}