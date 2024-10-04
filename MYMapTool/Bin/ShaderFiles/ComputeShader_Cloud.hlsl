// ComputeShader_Cloud.hlsl

// 상수 버퍼 정의
cbuffer CloudParams : register(b0)
{
    float g_CloudScale;
    float g_CloudDensity;
    float g_CloudSpeed;
    float g_Time;
}

// 텍스처 정의
Texture3D<float4> gInput : register(t0);
RWTexture3D<float4> gOutput : register(u0);

#define N 8
#define CacheSize (N + 2)
groupshared float4 gCache[CacheSize][CacheSize][CacheSize];

// 유틸리티 함수
float3 hash33(float3 p)
{
    p = frac(p * float3(443.8975, 397.2973, 491.1871));
    p += dot(p.zxy, p.yxz + 19.19);
    return frac(float3(p.x * p.y, p.z * p.x, p.y * p.z));
}

float noise(float3 p)
{
    float3 i = floor(p);
    float3 f = frac(p);
    f = f * f * (3.0 - 2.0 * f);

    return lerp(
        lerp(lerp(dot(hash33(i + float3(0, 0, 0)), f - float3(0, 0, 0)),
            dot(hash33(i + float3(1, 0, 0)), f - float3(1, 0, 0)), f.x),
            lerp(dot(hash33(i + float3(0, 1, 0)), f - float3(0, 1, 0)),
                dot(hash33(i + float3(1, 1, 0)), f - float3(1, 1, 0)), f.x), f.y),
        lerp(lerp(dot(hash33(i + float3(0, 0, 1)), f - float3(0, 0, 1)),
            dot(hash33(i + float3(1, 0, 1)), f - float3(1, 0, 1)), f.x),
            lerp(dot(hash33(i + float3(0, 1, 1)), f - float3(0, 1, 1)),
                dot(hash33(i + float3(1, 1, 1)), f - float3(1, 1, 1)), f.x), f.y), f.z);
}

float fbm(float3 p)
{
    float f = 0.0;
    f += 0.5000 * noise(p); p *= 2.01;
    f += 0.2500 * noise(p); p *= 2.02;
    f += 0.1250 * noise(p); p *= 2.03;
    f += 0.0625 * noise(p);
    return f;
}

[numthreads(N, N, N)]
void GenerateNoise(int3 groupThreadID : SV_GroupThreadID,
    int3 dispatchThreadID : SV_DispatchThreadID)
{
    float3 pos = float3(dispatchThreadID) / 64.0; // 텍스처 크기에 따라 조정
    pos *= g_CloudScale;
    pos.xy += g_Time * g_CloudSpeed;

    float perlin = fbm(pos);
    float worley = 1.0 - fbm(pos * 2.0);

    gOutput[dispatchThreadID] = float4(perlin, worley, 0, 0);
}

[numthreads(N, N, N)]
void CalculateDensity(int3 groupThreadID : SV_GroupThreadID,
    int3 dispatchThreadID : SV_DispatchThreadID)
{
    int3 globalIdx = dispatchThreadID;

    // 캐시에 데이터 로드
    gCache[groupThreadID.x + 1][groupThreadID.y + 1][groupThreadID.z + 1] = gInput[globalIdx];

    // 경계 조건 처리
    if (groupThreadID.x == 0)
        gCache[0][groupThreadID.y + 1][groupThreadID.z + 1] = gInput[max(globalIdx - int3(1, 0, 0), 0)];
    if (groupThreadID.x == N - 1)
        gCache[N + 1][groupThreadID.y + 1][groupThreadID.z + 1] = gInput[min(globalIdx + int3(1, 0, 0), 63)];

    if (groupThreadID.y == 0)
        gCache[groupThreadID.x + 1][0][groupThreadID.z + 1] = gInput[max(globalIdx - int3(0, 1, 0), 0)];
    if (groupThreadID.y == N - 1)
        gCache[groupThreadID.x + 1][N + 1][groupThreadID.z + 1] = gInput[min(globalIdx + int3(0, 1, 0), 63)];

    if (groupThreadID.z == 0)
        gCache[groupThreadID.x + 1][groupThreadID.y + 1][0] = gInput[max(globalIdx - int3(0, 0, 1), 0)];
    if (groupThreadID.z == N - 1)
        gCache[groupThreadID.x + 1][groupThreadID.y + 1][N + 1] = gInput[min(globalIdx + int3(0, 0, 1), 63)];

    GroupMemoryBarrierWithGroupSync();

    float4 noise = gCache[groupThreadID.x + 1][groupThreadID.y + 1][groupThreadID.z + 1];
    float perlin = noise.x;
    float worley = noise.y;

    float density = lerp(perlin, 1.0, worley);
    density = saturate(density - 0.3) * g_CloudDensity;

    gOutput[globalIdx] = float4(density, density, density, 1.0);
}

technique11 CloudGeneration
{
    pass GenerateNoise
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, GenerateNoise()));
    }

    pass CalculateDensity
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, CalculateDensity()));
    }
}