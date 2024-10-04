
#include "Engine_Shader_Defines.hlsli"

/* ������Ʈ ���̺�(������̺�) */
matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix, g_PrevWorldMatrix, g_PrevViewMatrix;

texture2D	g_BrushTexture;
texture2D	g_MaskTexture;
texture2D	g_DiffuseTexture[17];
//texture2D	g_NormalTexture[17];
texture2D	g_RoughnessTexture[17];

vector		g_vBrushPos = float4(30.f, 0.0f, 20.f, 1.f);
float		g_fBrushRange = 5.f;
float g_fWorldSize = 3075.f;

// ���� ���� �߰�
float g_fSnowGroundHeight;
float g_fSnowGroundHeightOffset;

bool g_MotionBlur = false;


struct VS_IN
{
    float3		vPosition : POSITION;
    float3		vNormal : NORMAL;
    float2		vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4		vPosition : SV_POSITION;
    float4		vNormal : NORMAL;
    float2		vTexcoord : TEXCOORD0;
    float4		vProjPos : TEXCOORD1;
    float4		vWorldPos : TEXCOORD2;
    float2		vVelocity : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT		Out = (VS_OUT)0;

    matrix		matWV, matWVP, matPrevWV, matPrevWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    matPrevWV = mul(g_PrevWorldMatrix, g_PrevViewMatrix);
    matPrevWVP = mul(matPrevWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

    vector vPrevPos = mul(float4(In.vPosition, 1.f), matPrevWVP);
    Out.vVelocity = (Out.vPosition.xy / Out.vPosition.w) - (vPrevPos.xy / vPrevPos.w);
    Out.vVelocity *= -0.3f;

    return Out;
}


struct PS_IN
{
    float4      vPosition : SV_POSITION;
    float4      vNormal : NORMAL;
    float2      vTexcoord : TEXCOORD0;
    float4      vProjPos : TEXCOORD1;
    float4      vWorldPos : TEXCOORD2;
    float2		vVelocity : TEXCOORD3;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vSpecular : SV_TARGET3;
    vector vEmissive : SV_TARGET4;
    vector vRoughness : SV_TARGET5;
    vector vMetalic : SV_TARGET6;
    float2 vVelocity : SV_TARGET7;
};

PS_OUT PS_MAIN(PS_IN In)
{

    PS_OUT      Out = (PS_OUT)0;

    // ��ְ��� �̿��� ��� ���
    float3 worldNormal = normalize(In.vNormal.xyz);
    float slope = 1.0f - dot(worldNormal, float3(0, 1, 0));

    // ���� ���� ��� (���� ��ǥ�� Y�� ���)
    float localHeight = In.vWorldPos.y;

    // ���� ��ǥ�� xz ���� �̿��Ͽ� �ؽ�ó ���� ��� ���
    float2 worldPosXZ = In.vWorldPos.xz;
    float distanceFromCenter = length(worldPosXZ) / g_fWorldSize;
    float grassBlend = saturate(distanceFromCenter * 2); // 0���� 1 ������ ������ ����

    // Diffuse �ؽ�ó ���ø�
    vector vGrass0 = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vGrass1 = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vGrass2 = g_DiffuseTexture[2].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vGrass3 = g_DiffuseTexture[3].Sample(LinearSampler, In.vTexcoord * 30.f);

    // Grass �ؽ�ó ����
    vector vLowDiffuse = lerp(vGrass0, vGrass3, grassBlend);

    vector vMidDiffuse = g_DiffuseTexture[4].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vHighDiffuse = g_DiffuseTexture[10].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vTopDiffuse = g_DiffuseTexture[13].Sample(LinearSampler, In.vTexcoord * 30.f);

    // Roughness �ؽ�ó ���ø�
    float vGrassRoughness0 = g_RoughnessTexture[0].Sample(LinearSampler, In.vTexcoord * 30.f).r;
    float vGrassRoughness1 = g_RoughnessTexture[1].Sample(LinearSampler, In.vTexcoord * 30.f).r;
    float vGrassRoughness2 = g_RoughnessTexture[2].Sample(LinearSampler, In.vTexcoord * 30.f).r;
    float vGrassRoughness3 = g_RoughnessTexture[3].Sample(LinearSampler, In.vTexcoord * 30.f).r;

    // Grass Roughness ����
    float vLowRoughness = lerp(vGrassRoughness0, vGrassRoughness3, grassBlend);

    float vMidRoughness = g_RoughnessTexture[4].Sample(LinearSampler, In.vTexcoord * 30.f).r;
    float vHighRoughness = g_RoughnessTexture[10].Sample(LinearSampler, In.vTexcoord * 30.f).r;
    float vTopRoughness = g_RoughnessTexture[13].Sample(LinearSampler, In.vTexcoord * 30.f).r;

    // ���̿� ���� �ε巯�� ����
    float heightBlend = smoothstep(g_fSnowGroundHeight - g_fSnowGroundHeightOffset,
        g_fSnowGroundHeight + g_fSnowGroundHeightOffset,
        localHeight);
    vector baseDiffuse = lerp(vLowDiffuse, vTopDiffuse, heightBlend);
    float baseRoughness = lerp(vLowRoughness, vTopRoughness, heightBlend);


    // ��� �ؽ�ó ���ø� (�ּ� ó��)
// vector vLowNormal = g_NormalTexture[0].Sample(LinearSampler, In.vTexcoord * 30.f);
// vector vMidNormal = g_NormalTexture[4].Sample(LinearSampler, In.vTexcoord * 30.f);
// vector vHighNormal = g_NormalTexture[10].Sample(LinearSampler, In.vTexcoord * 30.f);
// vector vTopNormal = g_NormalTexture[13].Sample(LinearSampler, In.vTexcoord * 30.f);
// 
// 
    // ��絵�� ���� ����
    vector vMtrlDiffuse;
    float finalRoughness;

    // ��絵�� ���� ����
    if (slope < 0.3) // ���� ���
    {
        float blend = smoothstep(0.0, 0.3, slope);
        vMtrlDiffuse = lerp(baseDiffuse, vMidDiffuse, blend);
        finalRoughness = lerp(baseRoughness, vMidRoughness, blend);
    }
    else if (slope < 0.7) // �߰� ���
    {
        float blend = smoothstep(0.3, 0.7, slope);
        vMtrlDiffuse = lerp(vMidDiffuse, vHighDiffuse, blend);
        finalRoughness = lerp(vMidRoughness, vHighRoughness, blend);
    }
    else // ���� ���
    {
        vMtrlDiffuse = vHighDiffuse;
        finalRoughness = vHighRoughness;
    }

    // �귯�� ���� (���� �ڵ� ����)
    vector vBrush = float4(0.0f, 0.f, 0.f, 0.f);
    if (g_vBrushPos.x - g_fBrushRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRange &&
        g_vBrushPos.z - g_fBrushRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRange)
    {
        float2 vTexcoord = (float2)0.f;
        vTexcoord.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
        vTexcoord.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);
        vBrush = g_BrushTexture.Sample(LinearSampler, vTexcoord);
    }

    // ���� ��ǻ�� ���� ���
    Out.vDiffuse = vMtrlDiffuse + vBrush * 0.5f;

    // �븻 ���
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    // ���� ����
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);
    Out.vRoughness = vector(finalRoughness, 0.f, 0.f, 1.f);


    if (g_MotionBlur)
    {
        Out.vVelocity = In.vVelocity;
    }

    return Out;
}

PS_OUT PS_WIRE(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    // ��ְ��� �̿��� ��� ���
    float3 worldNormal = normalize(In.vNormal.xyz);
    float slope = 1.0f - dot(worldNormal, float3(0, 1, 0));

    // �ؽ�ó ���ø�
    vector vLowDiffuse = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vMidDiffuse = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vHighDiffuse = g_DiffuseTexture[2].Sample(LinearSampler, In.vTexcoord * 30.f);

    //vector vLowNormal = g_NormalTexture[0].Sample(LinearSampler, In.vTexcoord * 30.f);
    //vector vMidNormal = g_NormalTexture[1].Sample(LinearSampler, In.vTexcoord * 30.f);
    //vector vHighNormal = g_NormalTexture[2].Sample(LinearSampler, In.vTexcoord * 30.f);

    // ��絵�� ���� ����
    vector vMtrlDiffuse, vNormalDesc;
    if (slope < 0.3) // ���� ���
    {
        float blend = smoothstep(0.0, 0.3, slope);
        vMtrlDiffuse = lerp(vLowDiffuse, vMidDiffuse, blend);
        //  vNormalDesc = lerp(vLowNormal, vMidNormal, blend);
    }
    else if (slope < 0.7) // �߰� ���
    {
        float blend = smoothstep(0.3, 0.7, slope);
        vMtrlDiffuse = lerp(vMidDiffuse, vHighDiffuse, blend);
        //  vNormalDesc = lerp(vMidNormal, vHighNormal, blend);
    }
    else // ���� ���
    {
        vMtrlDiffuse = vHighDiffuse;
        //  vNormalDesc = vHighNormal;
    }

    // �귯�� ���� (���� �ڵ� ����)
    vector vBrush = float4(0.0f, 0.f, 0.f, 0.f);
    if (g_vBrushPos.x - g_fBrushRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRange &&
        g_vBrushPos.z - g_fBrushRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRange)
    {
        float2 vTexcoord = (float2)0.f;
        vTexcoord.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
        vTexcoord.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);
        vBrush = g_BrushTexture.Sample(LinearSampler, vTexcoord);
    }

    // ���� ��ǻ�� ���� ���
   // Out.vDiffuse = vMtrlDiffuse + vBrush * 0.5f;
    Out.vDiffuse = vector(1.f, 0.f, 0.f, 1.f);

    // �븻 ���
 //   float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    // ���� ����
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);

    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

        pass WireFrame
    {
        SetRasterizerState(RS_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}

