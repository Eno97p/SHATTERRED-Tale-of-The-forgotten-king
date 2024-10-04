#include "Engine_Shader_Defines.hlsli"
#define UI0 1597334673U
#define UI1 3812015801U
#define UIF (1.0 / float(0xffffffffU))

/* 컨스턴트 테이블(상수테이블) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix, g_InvViewProjMatrix;

vector g_vCamPosition;

// Cloud specific variables
float g_CloudDensity = 0.8f;
float g_CloudScale = 0.01f;
float g_CloudSpeed = 0.1f;
float g_CloudHeight = 100.0f;
float3 g_CloudColor = float3(0.6f, 0.6f, 0.6f);
float g_fAccTime;

float4 g_vLightDiffuse = float4(0.76f, 0.8f, 0.95f, 1.0f);
float4 g_vLightAmbient = float4(0.2f, 0.2f, 0.3f, 1.0f);
float4 g_vLightSpecular = float4(1.0f, 1.0f, 1.0f, 1.0f);
float4 g_vLightDir = normalize(float4(-1.0f, -1.0f, -1.0f, 0.0f));

float4 g_vLightPosition;
float g_fLightRange;

// 셰이더 코드에 추가
float g_fCoarseStepSize;
float g_fFineStepSize;
int g_iMaxCoarseSteps = 64;
int g_iMaxFineSteps;
float g_fDensityThreshold;
float g_fAlphaThreshold;

float m_fPerlinWorleyMix;

int g_iPerlinOctaves;
float g_fPerlinFrequency;
float g_fWorleyFrequency;
float g_fPerlinPersistence;
float g_fPerlinLacunarity;
float g_fWorleyJitter;
float g_fPerlinWorleyMix;
float g_fNoiseRemapLower;
float g_fNoiseRemapUpper;

//Noise 3D
texture3D g_3DNoiseTexture;

//FOR REFLECTION
float g_fReflectionPlaneHeight;
float g_fReflectionQuality;
float g_fReflectionOpacity;
float g_fReflectionDensityScale;
float4 g_vBaseSkyColor;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vWorldPos : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float3 vLocalPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix).xyz;
    Out.vProjPos = Out.vPosition;
    Out.vLocalPos = In.vPosition;  // 로컬 위치 전달

    return Out;
}

float3 hash33(float3 p)
{
    uint3 q = uint3(int3(p)) * uint3(UI0, UI1, 2798796415U);
    q = (q.x ^ q.y ^ q.z) * uint3(UI0, UI1, 2798796415U);
    return -1.0 + 2.0 * float3(q) * UIF;
}

float remap(float x, float a, float b, float c, float d)
{
    return (((x - a) / (b - a)) * (d - c)) + c;
}

float gradientNoise(float3 x, float freq)
{
    float3 p = floor(x);
    float3 w = frac(x);

    float3 u = w * w * w * (w * (w * 6.0 - 15.0) + 10.0);

    float3 ga = hash33(fmod(p + float3(0.0, 0.0, 0.0), freq));
    float3 gb = hash33(fmod(p + float3(1.0, 0.0, 0.0), freq));
    float3 gc = hash33(fmod(p + float3(0.0, 1.0, 0.0), freq));
    float3 gd = hash33(fmod(p + float3(1.0, 1.0, 0.0), freq));
    float3 ge = hash33(fmod(p + float3(0.0, 0.0, 1.0), freq));
    float3 gf = hash33(fmod(p + float3(1.0, 0.0, 1.0), freq));
    float3 gg = hash33(fmod(p + float3(0.0, 1.0, 1.0), freq));
    float3 gh = hash33(fmod(p + float3(1.0, 1.0, 1.0), freq));

    float va = dot(ga, w - float3(0.0, 0.0, 0.0));
    float vb = dot(gb, w - float3(1.0, 0.0, 0.0));
    float vc = dot(gc, w - float3(0.0, 1.0, 0.0));
    float vd = dot(gd, w - float3(1.0, 1.0, 0.0));
    float ve = dot(ge, w - float3(0.0, 0.0, 1.0));
    float vf = dot(gf, w - float3(1.0, 0.0, 1.0));
    float vg = dot(gg, w - float3(0.0, 1.0, 1.0));
    float vh = dot(gh, w - float3(1.0, 1.0, 1.0));

    return va +
        u.x * (vb - va) +
        u.y * (vc - va) +
        u.z * (ve - va) +
        u.x * u.y * (va - vb - vc + vd) +
        u.y * u.z * (va - vc - ve + vg) +
        u.z * u.x * (va - vb - ve + vf) +
        u.x * u.y * u.z * (-va + vb + vc - vd + ve - vf - vg + vh);
}

float worleyNoise(float3 uv, float freq)
{
    float3 id = floor(uv);
    float3 p = frac(uv);

    float minDist = 10000.0;
    for (float x = -1.0; x <= 1.0; ++x)
    {
        for (float y = -1.0; y <= 1.0; ++y)
        {
            for (float z = -1.0; z <= 1.0; ++z)
            {
                float3 offset = float3(x, y, z);
                float3 h = hash33(fmod(id + offset, float3(freq, freq, freq)));
                h = (h - 0.5) * g_fWorleyJitter + 0.5;  // Apply jitter
                h += offset;
                float3 d = p - h;
                minDist = min(minDist, dot(d, d));
            }
        }
    }

    return 1.0 - minDist;
}
float perlinfbm(float3 p, float freq, int octaves)
{
    float G = g_fPerlinPersistence;
    float amp = 1.0;
    float noise = 0.0;
    for (int i = 0; i < octaves; ++i)
    {
        noise += amp * gradientNoise(p * freq, freq);
        freq *= g_fPerlinLacunarity;
        amp *= G;
    }

    return noise;
}

float worleyFbm(float3 p, float freq)
{
    return worleyNoise(p * freq, freq) * 0.625 +
        worleyNoise(p * freq * 2.0, freq * 2.0) * 0.25 +
        worleyNoise(p * freq * 4.0, freq * 4.0) * 0.125;
}

float calculateCloudDensity(float3 position, float time)
{
    float perlin = perlinfbm(position + float3(time * 0.1, 0, 0), g_fPerlinFrequency, g_iPerlinOctaves);
    float worley = worleyFbm(position + float3(time * 0.05, 0, 0), g_fWorleyFrequency);

    float noise = lerp(perlin, worley, g_fPerlinWorleyMix);
    noise = remap(noise, g_fNoiseRemapLower, g_fNoiseRemapUpper, 0.0, 1.0);

    float cloud = remap(noise, 0.0, 1.0, 0.0, 1.0);
    cloud = pow(cloud, 1.5); // 구름 형태를 더 뚜렷하게

    return cloud * g_CloudDensity * 2.0; // 밀도 증가
}

float HenyeyGreenstein(float cosTheta, float g)
{
    float g2 = g * g;
    return (1.0 - g2) / (4.0 * 3.14159 * pow(1.0 + g2 - 2.0 * g * cosTheta, 1.5));
}

float3 calculatePointLightEnergy(float3 position, float3 lightPos, float lightRange, float density, float3 viewDir)
{
    float3 lightDir = normalize(lightPos - position);
    float distanceToLight = length(lightPos - position);

    const float LIGHT_ABSORB_COEFF = 0.5;
    const float LIGHT_SCATTER_COEFF = 0.5;
    const float g = 0.2; // forward scattering parameter

    float cosTheta = dot(lightDir, viewDir);
    float phase = HenyeyGreenstein(cosTheta, g);

    float beerLambert = exp(-density * LIGHT_ABSORB_COEFF);
    float inscattering = (1.0 - exp(-density * LIGHT_SCATTER_COEFF)) * phase;

    // 포인트 라이트의 감쇠 계산
    float attenuation = 1.0 - smoothstep(0.0, lightRange, distanceToLight);

    return g_vLightDiffuse.rgb * (beerLambert + inscattering) * attenuation;
}

struct PS_OUT
{
    float4 vColor : SV_Target0;
};

float sphereTrace(float3 start, float3 dir, float maxDist, float stepSize)
{
    float t = 0.0;
    for (int i = 0; i < 128; i++)
    {
        float3 pos = start + t * dir;
        float density = calculateCloudDensity(pos * g_CloudScale, g_fAccTime * g_CloudSpeed);
        if (density > 0.01)
        {
            return t;
        }
        t += stepSize;
        if (t > maxDist) break;
    }
    return maxDist;
}

float coarseSpherTrace(float3 start, float3 dir, float maxDist, float stepSize)
{
    float t = 0.0;
    for (int i = 0; i < 32; i++) // 반복 횟수 감소
    {
        float3 pos = start + t * dir;
        float density = calculateCloudDensity(pos * g_CloudScale, g_fAccTime * g_CloudSpeed);
        if (density > 0.05) // 임계값 증가
        {
            return t;
        }
        t += stepSize * 2.0; // 스텝 크기 증가
        if (t > maxDist) break;
    }
    return maxDist;
}

float fineSpherTrace(float3 start, float3 dir, float maxDist, float stepSize)
{
    float t = 0.0;
    for (int i = 0; i < 16; i++) // 정밀 반복 횟수 제한
    {
        float3 pos = start + t * dir;
        float density = calculateCloudDensity(pos * g_CloudScale, g_fAccTime * g_CloudSpeed);
        if (density > 0.01)
        {
            return t;
        }
        t += stepSize * 0.5; // 더 작은 스텝 크기
        if (t > maxDist) break;
    }
    return maxDist;
}

PS_OUT PS_MAIN(VS_OUT In)
{
    PS_OUT Out = (PS_OUT)0;

    float3 worldPos = In.vWorldPos;
    float3 viewDir = normalize(worldPos - g_vCamPosition.xyz);
    float time = g_fAccTime;
    const float MAX_DIST = 100.0;
    float4 cloudColor = float4(0, 0, 0, 0);

    // 대략적인 Sphere Tracing
    float coarseT = coarseSpherTrace(worldPos, viewDir, MAX_DIST, g_fCoarseStepSize);
    if (coarseT >= MAX_DIST)
    {
        discard;
        return Out;
    }

    // 정밀한 Sphere Tracing
    float t = fineSpherTrace(worldPos + (coarseT - g_fCoarseStepSize) * viewDir, viewDir, g_fCoarseStepSize * 2, g_fFineStepSize);
    t += coarseT - g_fCoarseStepSize;

    // 레이 마칭 (기존 코드와 유사)
    float3 currentPos = worldPos + t * viewDir;
    float totalDensity = 0.0;

    [loop]
        for (int j = 0; j < g_iMaxFineSteps; j++)
        {
            float density = calculateCloudDensity(currentPos * g_CloudScale, time * g_CloudSpeed);
            if (density > 0.01)
            {
                totalDensity += density * g_fFineStepSize;
                float3 lightEnergy = calculatePointLightEnergy(currentPos, g_vLightPosition, g_fLightRange, density, viewDir);
                float3 color = g_CloudColor * lightEnergy * density;
                float transmittance = exp(-density * g_fFineStepSize);
                cloudColor.rgb += color * (1.0 - cloudColor.a) * transmittance;
                cloudColor.a += density * (1.0 - cloudColor.a) * 0.1;
                if (cloudColor.a > g_fAlphaThreshold) break;
            }
            currentPos += viewDir * g_fFineStepSize;
            t += g_fFineStepSize;
            if (t > MAX_DIST) break;
        }

    cloudColor.a = 1.0 - exp(-totalDensity * 0.1);
    if (cloudColor.a < 0.01)
        discard;

    float3 ambient = g_vLightAmbient.rgb * g_CloudColor * 0.1;
    cloudColor.rgb += ambient * (1.0 - cloudColor.a);
    cloudColor.rgb = pow(cloudColor.rgb, 1.0 / 2.2);

    Out.vColor = cloudColor;
    return Out;
}

float3 mod289(float3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
float4 mod289(float4 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
float4 permute(float4 x) { return mod289(((x * 34.0) + 1.0) * x); }
float4 taylorInvSqrt(float4 r) { return 1.79284291400159 - 0.85373472095314 * r; }

float snoise(float3 v)
{
    const float2 C = float2(1.0 / 6.0, 1.0 / 3.0);
    const float4 D = float4(0.0, 0.5, 1.0, 2.0);

    // First corner
    float3 i = floor(v + dot(v, C.yyy));
    float3 x0 = v - i + dot(i, C.xxx);

    // Other corners
    float3 g = step(x0.yzx, x0.xyz);
    float3 l = 1.0 - g;
    float3 i1 = min(g.xyz, l.zxy);
    float3 i2 = max(g.xyz, l.zxy);

    float3 x1 = x0 - i1 + C.xxx;
    float3 x2 = x0 - i2 + C.yyy;
    float3 x3 = x0 - D.yyy;

    // Permutations
    i = mod289(i);
    float4 p = permute(permute(permute(
        i.z + float4(0.0, i1.z, i2.z, 1.0))
        + i.y + float4(0.0, i1.y, i2.y, 1.0))
        + i.x + float4(0.0, i1.x, i2.x, 1.0));

    // Gradients: 7x7 points over a square, mapped onto an octahedron.
    // The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    float n_ = 0.142857142857;
    float3 ns = n_ * D.wyz - D.xzx;

    float4 j = p - 49.0 * floor(p * ns.z * ns.z);

    float4 x_ = floor(j * ns.z);
    float4 y_ = floor(j - 7.0 * x_);

    float4 x = x_ * ns.x + ns.yyyy;
    float4 y = y_ * ns.x + ns.yyyy;
    float4 h = 1.0 - abs(x) - abs(y);

    float4 b0 = float4(x.xy, y.xy);
    float4 b1 = float4(x.zw, y.zw);

    float4 s0 = floor(b0) * 2.0 + 1.0;
    float4 s1 = floor(b1) * 2.0 + 1.0;
    float4 sh = -step(h, float4(0.0, 0.0, 0.0, 0.0));

    float4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
    float4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

    float3 p0 = float3(a0.xy, h.x);
    float3 p1 = float3(a0.zw, h.y);
    float3 p2 = float3(a1.xy, h.z);
    float3 p3 = float3(a1.zw, h.w);

    // Normalise gradients
    float4 norm = taylorInvSqrt(float4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    float4 m = max(0.6 - float4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0);
    m = m * m;
    return 42.0 * dot(m * m, float4(dot(p0, x0), dot(p1, x1), dot(p2, x2), dot(p3, x3)));
}

float calculateCloudDensityFromTexture(float3 position, float time)
{
    // 기본 구조를 위한 고정된 샘플링 위치
    float3 basePos = position * g_CloudScale;

    // 시간에 따른 변화를 위한 오프셋
    float3 timeOffset = float3(time * g_CloudSpeed * 0.1, time * g_CloudSpeed * 0.05, time * g_CloudSpeed * 0.07);

    // 부드러운 타일링을 위한 노이즈 추가 (시간에 따라 천천히 변화)
    float3 noiseOffset = float3(
        snoise(basePos * 0.01 + timeOffset * 0.1),
        snoise(basePos * 0.01 + timeOffset * 0.1 + 100),
        snoise(basePos * 0.01 + timeOffset * 0.1 + 200)
    ) * 5.0;

    // 기본 구조와 시간 변화를 결합
    float3 samplePos = basePos + noiseOffset;

    // 노이즈 데이터 샘플링
    float4 noiseData = g_3DNoiseTexture.SampleLevel(CloudSampler, frac(samplePos), 0);
    float perlin = noiseData.r;
    float worley = noiseData.g;
    float detailNoise = noiseData.b;

    // Perlin FBM 시뮬레이션 (기본 구조 유지)
    float perlinFbm = perlin;
    for (int i = 1; i < g_iPerlinOctaves; ++i)
    {
        float freq = pow(g_fPerlinLacunarity, float(i));
        float amp = pow(g_fPerlinPersistence, float(i));
        perlinFbm += g_3DNoiseTexture.SampleLevel(CloudSampler, frac(basePos * freq), 0).r * amp;
    }
    perlinFbm = saturate(perlinFbm);

    // Worley FBM 시뮬레이션 (시간에 따른 세부 변화 추가)
    float worleyFbm = worley * 0.625 +
        g_3DNoiseTexture.SampleLevel(CloudSampler, frac(samplePos * 2.0), 0).g * 0.25 +
        g_3DNoiseTexture.SampleLevel(CloudSampler, frac(samplePos * 4.0), 0).g * 0.125;

    // 기본 구름 형태 생성
    float baseCloud = lerp(perlinFbm, worleyFbm, g_fPerlinWorleyMix);
    baseCloud = remap(baseCloud, g_fNoiseRemapLower, g_fNoiseRemapUpper, 0.0, 1.0);

    // 시간에 따른 세부 변화 적용
    float timeDetailNoise = snoise(samplePos * 0.1 + timeOffset);
    float detailModifier = lerp(detailNoise, timeDetailNoise, 0.3);
    baseCloud = remap(baseCloud, detailModifier * 0.2, 1.0, 0.0, 1.0);

    // 구름 형태 조정
    float cloud = pow(baseCloud, g_CloudDensity);

    // 최종 밀도 계산 및 클램핑
    return saturate(cloud) * g_CloudDensity * 2.0;
}

PS_OUT PS_TEX(VS_OUT In)
{
    PS_OUT Out = (PS_OUT)0;
    float3 worldPos = In.vWorldPos;
    float3 viewDir = normalize(worldPos - g_vCamPosition.xyz);
    float time = g_fAccTime;
    const float MAX_DIST = 100.0;
    float4 cloudColor = float4(0, 0, 0, 0);

    // 구름의 중심점 정의 (x와 z만 사용)
    float2 cloudCenter = float2(-71.919f, -49.122f);

    // 현재 위치와 구름 중심 사이의 XZ 평면상의 거리 계산
    float distFromCenter = length(worldPos.xz - cloudCenter);

    // 최대 페이드 거리 (이 값은 돔의 크기에 따라 조정 필요)
    float maxFadeDistance = 2800.f;

    // XZ 평면상의 거리에 따른 페이드 팩터 계산 (0: 완전 투명, 1: 완전 불투명)
    float fadeFactor = saturate(1.0 - (distFromCenter / maxFadeDistance));

    // fadeFactor가 0이면 early out
    if (fadeFactor == 0.0)
    {
        discard;
        return Out;
    }

    // 대략적인 레이 마칭
    float coarseT = 0.0;
    float coarseDensity = 0.0;
    for (int i = 0; i < g_iMaxCoarseSteps; i++)
    {
        float3 pos = worldPos + coarseT * viewDir;
        coarseDensity = calculateCloudDensityFromTexture(pos, time);
        if (coarseDensity > g_fDensityThreshold)
            break;
        coarseT += g_fCoarseStepSize;
        if (coarseT > MAX_DIST)
        {
            discard;
            return Out;
        }
    }

    // 정밀한 레이 마칭 (적응형 샘플링)
    float t = max(0, coarseT - g_fCoarseStepSize);
    float3 currentPos = worldPos + t * viewDir;
    float totalDensity = 0.0;
    float stepSize = (coarseDensity > g_fDensityThreshold) ? g_fFineStepSize : g_fCoarseStepSize;
    int maxSteps = (coarseDensity > g_fDensityThreshold) ? g_iMaxFineSteps : g_iMaxCoarseSteps;

    [loop]
        for (int j = 0; j < maxSteps; j++)
        {
            float density = calculateCloudDensityFromTexture(currentPos, time);
            if (density > 0.01)
            {
                totalDensity += density * stepSize;
                float3 lightEnergy = calculatePointLightEnergy(currentPos, g_vLightPosition, g_fLightRange, density, viewDir);
                float3 color = g_CloudColor * lightEnergy * density;
                float transmittance = exp(-density * stepSize);
                cloudColor.rgb += color * (1.0 - cloudColor.a) * transmittance;
                cloudColor.a += density * (1.0 - cloudColor.a) * 0.1;
                if (cloudColor.a > g_fAlphaThreshold) break;
            }
            currentPos += viewDir * stepSize;
            t += stepSize;
            if (t > MAX_DIST) break;
        }

    cloudColor.a = 1.0 - exp(-totalDensity * 0.1);

    // 페이드 팩터 적용
    cloudColor.a *= fadeFactor;

    // 알파값이 0이면 discard
    if (cloudColor.a < 0.1f )
    {
        discard;
    }

    float3 ambient = g_vLightAmbient.rgb * g_CloudColor * 0.1;
    cloudColor.rgb += ambient * (1.0 - cloudColor.a);
    cloudColor.rgb = pow(cloudColor.rgb, 1.0 / 2.2);

    if (cloudColor.r < 0.5f)
    {
        discard;
    }
    Out.vColor = cloudColor;
    return Out;
}

 PS_OUT PS_REFLECTION(VS_OUT In)
{
    PS_OUT Out = (PS_OUT)0;
    float3 worldPos = In.vWorldPos;
    float3 viewDir = normalize(worldPos - g_vCamPosition.xyz);
    float time = g_fAccTime;
    const float MAX_DIST = 100.0;
    float4 cloudColor = float4(0, 0, 0, 0);

    // 구름의 중심점 정의 (x와 z만 사용)
    float2 cloudCenter = float2(-71.919f, -49.122f);

    // 현재 위치와 구름 중심 사이의 XZ 평면상의 거리 계산
    float distFromCenter = length(worldPos.xz - cloudCenter);

    // 최대 페이드 거리 (이 값은 돔의 크기에 따라 조정 필요)
    float maxFadeDistance = 2800.f;

    // XZ 평면상의 거리에 따른 페이드 팩터 계산 (0: 완전 투명, 1: 완전 불투명)
    float fadeFactor = saturate(1.0 - (distFromCenter / maxFadeDistance));

    // fadeFactor가 0이면 early out
    if (fadeFactor == 0.0)
    {
        discard;
        return Out;
    }

    // 대략적인 레이 마칭
    float coarseT = 0.0;
    float coarseDensity = 0.0;
    for (int i = 0; i < g_iMaxCoarseSteps; i++)
    {
        float3 pos = worldPos + coarseT * viewDir;
        coarseDensity = calculateCloudDensityFromTexture(pos, time) * g_fReflectionDensityScale;
        if (coarseDensity > g_fDensityThreshold)
            break;
        coarseT += g_fCoarseStepSize;
        if (coarseT > MAX_DIST)
        {
            discard;
            return Out;
        }
    }

    // 정밀한 레이 마칭 (적응형 샘플링)
    float t = max(0, coarseT - g_fCoarseStepSize);
    float3 currentPos = worldPos + t * viewDir;
    float totalDensity = 0.0;
    float stepSize = (coarseDensity > g_fDensityThreshold) ? g_fFineStepSize : g_fCoarseStepSize;
    int maxSteps = (coarseDensity > g_fDensityThreshold) ? g_iMaxFineSteps : g_iMaxCoarseSteps;

    [loop]
        for (int j = 0; j < maxSteps; j++)
        {
            //precise    : 부동 소수점 정밀도 유지
             float density = calculateCloudDensityFromTexture(currentPos, time) * g_fReflectionDensityScale;

            //[branch]        //조건문 최적화 방지
            if (density > 0.01)
            {
                totalDensity += density * stepSize;
                float3 lightEnergy = calculatePointLightEnergy(currentPos, g_vLightPosition, g_fLightRange, density, viewDir);
                float3 color = g_CloudColor * lightEnergy * density;


                //// 테일러 급수를 사용한 exp 근사
                //float x = -density * stepSize;
                //float transmittance = 1.0f + x + 0.5f * x * x + (1.0f / 6.0f) * x * x * x + (1.0f / 24.0f) * x * x * x * x;     //4차항 까지 근사
                //transmittance = max(transmittance, 0.0f); // 음수 방지

                precise  float transmittance = exp(-density * stepSize);
                cloudColor.rgb += color * (1.0 - cloudColor.a) * transmittance;
                cloudColor.a += density * (1.0 - cloudColor.a) * 0.1;
                if (cloudColor.a > g_fAlphaThreshold) break;
            }
            currentPos += viewDir * stepSize;
            t += stepSize;
            if (t > MAX_DIST) break;
        }

    cloudColor.a = 1.0 - exp(-totalDensity * 0.1);

    // 페이드 팩터 적용
    cloudColor.a *= fadeFactor;

    // 알파값이 낮으면 기본 하늘색 반환
    if (cloudColor.a < 0.1f)
    {
        Out.vColor = float4(g_vBaseSkyColor.rgb, 0.8f);
    }
    else
    {
        float3 ambient = g_vLightAmbient.rgb * g_CloudColor * 0.1;
        cloudColor.rgb += ambient * (1.0 - cloudColor.a);
        cloudColor.rgb = pow(cloudColor.rgb, 1.0 / 2.2) * 2.f;

        // 반사에 대한 불투명도 조정
        cloudColor.a *= g_fReflectionOpacity;

        Out.vColor = cloudColor * 1.4f;
    }

    return Out;
}






PS_OUT PS_REFLECTION_OPTIMIZE(VS_OUT In)
{
    PS_OUT Out = (PS_OUT)0;
    float3 worldPos = In.vWorldPos;
    float3 viewDir = normalize(worldPos - g_vCamPosition.xyz);
    float time = g_fAccTime;
    const float MAX_DIST = 100.0;
    float4 cloudColor = float4(0, 0, 0, 0);

    // 구름의 중심점 정의 (x와 z만 사용)
    float2 cloudCenter = float2(-71.919f, -49.122f);

    // 현재 위치와 구름 중심 사이의 XZ 평면상의 거리 계산
    float distFromCenter = length(worldPos.xz - cloudCenter);

    // 최대 페이드 거리 (이 값은 돔의 크기에 따라 조정 필요)
    float maxFadeDistance = 2800.f;

    // XZ 평면상의 거리에 따른 페이드 팩터 계산 (0: 완전 투명, 1: 완전 불투명)
    float fadeFactor = saturate(1.0 - (distFromCenter / maxFadeDistance));

    // fadeFactor가 0이면 early out
    if (fadeFactor == 0.0)
    {
        discard;
        return Out;
    }

    // 대략적인 레이 마칭
    float coarseT = 0.0;
    float coarseDensity = 0.0;
    for (int i = 0; i < g_iMaxCoarseSteps; i++)
    {
        float3 pos = worldPos + coarseT * viewDir;
        coarseDensity = calculateCloudDensityFromTexture(pos, time) * g_fReflectionDensityScale;
        if (coarseDensity > g_fDensityThreshold)
            break;
        coarseT += g_fCoarseStepSize;
        if (coarseT > MAX_DIST)
        {
            discard;
            return Out;
        }
    }

    // 정밀한 레이 마칭 (적응형 샘플링)
    float t = max(0, coarseT - g_fCoarseStepSize);
    float3 currentPos = worldPos + t * viewDir;
    float totalDensity = 0.0;
    float stepSize = (coarseDensity > g_fDensityThreshold) ? g_fFineStepSize : g_fCoarseStepSize;
    int maxSteps = (coarseDensity > g_fDensityThreshold) ? g_iMaxFineSteps : g_iMaxCoarseSteps;

    
        for (int j = 0; j < maxSteps; j++)
        {
            //precise    : 부동 소수점 정밀도 유지
            float density = calculateCloudDensityFromTexture(currentPos, time) * g_fReflectionDensityScale;

            //[branch]        //조건문 최적화 방지
            if (density > 0.01)
            {
                totalDensity += density * stepSize;
                float3 lightEnergy = calculatePointLightEnergy(currentPos, g_vLightPosition, g_fLightRange, density, viewDir);
                float3 color = g_CloudColor * lightEnergy * density;


                //// 테일러 급수를 사용한 exp 근사
                //float x = -density * stepSize;
                //float transmittance = 1.0f + x + 0.5f * x * x + (1.0f / 6.0f) * x * x * x + (1.0f / 24.0f) * x * x * x * x;     //4차항 까지 근사
                //transmittance = max(transmittance, 0.0f); // 음수 방지

                 float transmittance = exp(-density * stepSize);
                cloudColor.rgb += color * (1.0 - cloudColor.a) * transmittance;
                cloudColor.a += density * (1.0 - cloudColor.a) * 0.1;
                if (cloudColor.a > g_fAlphaThreshold) break;
            }
            currentPos += viewDir * stepSize;
            t += stepSize;
            if (t > MAX_DIST) break;
        }

    cloudColor.a = 1.0 - exp(-totalDensity * 0.1);

    // 페이드 팩터 적용
    cloudColor.a *= fadeFactor;

    // 알파값이 낮으면 기본 하늘색 반환
    if (cloudColor.a < 0.1f)
    {
        Out.vColor = float4(g_vBaseSkyColor.rgb, 0.8f);
    }
    else
    {
        float3 ambient = g_vLightAmbient.rgb * g_CloudColor * 0.1;
        cloudColor.rgb += ambient * (1.0 - cloudColor.a);
        cloudColor.rgb = pow(cloudColor.rgb, 1.0 / 2.2) * 2.f;

        // 반사에 대한 불투명도 조정
        cloudColor.a *= g_fReflectionOpacity;

        Out.vColor = cloudColor * 1.4f;
    }

    return Out;
}







technique11 DefaultTechnique
{
    pass Cloud
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0); // 깊이 쓰기 비활성화
        SetBlendState(CloudBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Cloud_Tex
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0); // 깊이 쓰기 비활성화
        SetBlendState(CloudBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_TEX();
    }

    pass CloudReflection
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0); // 깊이 테스트 활성화
        SetBlendState(CloudBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_REFLECTION();
    }

      pass CloudReflection_OptiMize
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0); // 깊이 테스트 활성화
        SetBlendState(CloudBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_REFLECTION_OPTIMIZE();
    }

}