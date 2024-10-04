#include "Engine_Shader_Defines.hlsli"

/* ������Ʈ ���̺�(������̺�) */
matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix      g_GodRayViewMatrix, g_GodRayProjMatrix;
matrix      g_WorldMatrixInv, g_ViewMatrixInv, g_ProjMatrixInv;
matrix      g_LightViewMatrix, g_LightProjMatrix;
matrix      Test_g_LightViewMatrix, Test_g_LightProjMatrix;
vector      g_vLightDir;
vector      g_vLightPos;
float      g_fBRIS;
float      g_fMirror;
float      g_fLightRange;

float      g_fInnerAngle;
float      g_fOuterAngle;

vector      g_vLightDiffuse;
vector      g_vLightAmbient;
vector      g_vLightSpecular;


float g_fGodraysDestiny = 0.5f;     //���� �е�
float g_fGodraysWeight = 0.5f;      //���� ���
float g_fGodraysDecay = 0.95f;      //���� ����
float g_fGodraysExposure = 0.5f;      //���� ����
texture2D g_SunTexture;

//�̹ο� �߰� 240621 1638
float      g_fShadowThreshold = 0.8f;

vector      g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector      g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector      g_vCamPosition;

vector      g_vFogColor;
float      g_fFogRange;
float      g_fFogHeightFalloff;
float      g_fFogGlobalDensity;
float      g_fFogTimeOffset;

float      g_fNoiseSize;
float      g_fNoiseIntensity;

vector      g_vFogColor2;
float      g_fFogBlendFactor;
float      g_fFogTimeOffset2;
float      g_fNoiseSize2;
float      g_fNoiseIntensity2;

//�̹ο� �߰� 240811 2121PM
texture2D g_CausticTexture;
float g_fWaveStrength = 0.02f;  // ���÷��� �ĵ� ����
float g_fWaveFrequency = 5.0f;  // ���÷��� �ĵ� ���ļ�
float g_fWaveTimeOffset = 1.0f;
float g_fFresnelPower = 5.0f;  // ���÷��� �ĵ� ���ļ�

texture2D   g_Texture;
texture2D   g_MaskTexture;
texture2D   g_NormalTexture;
texture2D   g_DiffuseTexture;
texture2D   g_ShadeTexture;
texture2D   g_DepthTexture;
texture2D   g_LightDepthTexture;
texture2D   g_ShadowTexture;
texture2D   g_Shadow_MoveTexture;
texture2D   g_Shadow_NotMoveTexture;
texture2D   g_SpecularTexture;
texture2D g_DecalDepthTexture;
texture2D   g_GodRayTexture;

texture2D   g_EmissiveTexture;
texture2D   g_RoughnessTexture;
texture2D   g_MetalicTexture;

texture2D   g_SpecularMapTexture;

texture2D   g_EffectTexture;
texture2D   g_MirrorTexture;
texture2D   g_DistortionTexture;
texture2D   g_BlurTexture;

texture2D   g_LUTTexture;
texture2D   g_ResultTexture;
float      g_Time = 0.f;
float      g_Value = 0.1f;

float g_fTexW = 1280.0f;
float g_fTexH = 720.0f;
int     g_BlurNum = 1;

int g_TestBool;
float g_fFar;
//HBAO
texture2D g_HBAOTexture;


static const float g_fWeight[13] =
{
   0.0044, 0.0175, 0.0540, 0.1295, 0.2420, 0.3521, 0.3989, 0.3521, 0.2420, 0.1295, 0.0540, 0.0175, 0.0044
   //0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1,
   //0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};

static const float g_fWeight2[52] =
{
   0.0001, 0.0003, 0.0007, 0.0012, 0.0020, 0.0031, 0.0044, 0.0062, 0.0084, 0.0111, 0.0143, 0.0175,
0.0220, 0.0271, 0.0329, 0.0396, 0.0471, 0.0540, 0.0630, 0.0731, 0.0844, 0.0969, 0.1104, 0.1295,
0.2420, 0.3521, 0.3989, 0.3521, 0.2420, 0.1295, 0.1104, 0.0969, 0.0844, 0.0731, 0.0630, 0.0540,
0.0471, 0.0396, 0.0329, 0.0271, 0.0220, 0.0175, 0.0143, 0.0111, 0.0084, 0.0062, 0.0044, 0.0031,
0.0020, 0.0012, 0.0007, 0.0003
};


static const float g_fWeight3[260] =
{
   0.0000, 0.0000, 0.0000, 0.0000, 0.0001, 0.0001, 0.0001, 0.0001, 0.0002, 0.0002,
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
   0.0007, 0.0006, 0.0005, 0.0004, 0.0003, 0.0003, 0.0002, 0.0002, 0.0001, 0.0001
};


static const float g_fTotal = 1.5f;

struct VS_IN
{
    float3      vPosition : POSITION;
    float2      vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4      vPosition : SV_POSITION;
    float2      vTexcoord : TEXCOORD0;
};

/* ���� ���̴� :  /*
/* 1. ������ ��ġ ��ȯ(����, ��, ����).*/
/* 2. ������ ���������� �����Ѵ�. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT      Out = (VS_OUT)0;

    matrix      matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

/* TriangleList�� ��� : ���� ������ �޾Ƽ� w�����⸦ �� ������ ���ؼ� �����Ѵ�. */
/* ����Ʈ(��������ǥ��) ��ȯ. */
/* �����Ͷ����� : �������� �ѷ��׿��� �ȼ��� ������, ������ ���������Ͽ� �����. -> �ȼ��� ���������!!!!!!!!!!!! */
struct PS_IN
{
    float4      vPosition : SV_POSITION;
    float2      vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector      vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT      Out = (PS_OUT)0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);


    return Out;
}

struct PS_OUT_LIGHT
{
    vector      vShade : SV_TARGET0;
    vector      vSpecular : SV_TARGET1;
};


//�������
cbuffer LightData : register(b0)
{
    float2 screenSpacePosition; // ������ ȭ�� ���� ��ġ
    float godraysDensity;       // GodRays �е�
    float godraysWeight;        // GodRays ����ġ
    float godraysDecay;         // GodRays ����
    float godraysExposure;      // GodRays ����

}




PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);


    float hbao = g_HBAOTexture.Sample(LinearSampler, In.vTexcoord).r;

    // ��� ���Ͱ� ���������� ���ǰ� �ִ��� Ȯ��
    if (length(vNormal.xyz) == 0)
        return Out;

    // ���� ���
    float3 lightDir = normalize(g_vLightDir);
    float3 normal = normalize(vNormal.xyz);
    float3 lightAmbient = g_vLightAmbient * g_vMtrlAmbient;
    float3 lightDiffuse = g_vLightDiffuse * saturate(max(dot(-lightDir, normal), 0.f));
    Out.vShade = float4((lightDiffuse + lightAmbient) /** hbao*/, 1.f);




    // ���� �ؽ�ó ���ø�
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);

    // ���� ��ǥ ���
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;
    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    // ����ŧ�� ���
    vector vReflect = float4(reflect(-lightDir, normal), 1.f);
    vector vLook = normalize(vWorldPos - g_vCamPosition);

    vector vSpecularMap = g_SpecularMapTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
    vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    float roughness = vRoughness.r; // �����Ͻ� ��

    float NdotV = max(dot(normal, vLook), 0.f);
    float NdotL = max(dot(normal, lightDir), 0.f);
    float a = roughness * roughness;
    float a2 = a * a;

    float D = a2 / (3.14f * pow(NdotL * NdotL * (a2 - 1.f) + 1.f, 2.f));
    float F = pow(1.f - max(dot(vReflect, vLook), 0.f), 5.f);
    float G = min(1.f, min((2.f * NdotV * NdotL) / (NdotV + NdotL), (2.f * NdotL * (1.f - roughness)) / (NdotV * NdotL)));

    float specular = (D * F * G) / (4.f * NdotV * NdotL + 0.001f);

    float metallic = vMetalic.r;

    float kS = metallic * specular + (1.f - metallic) * 0.04f;
    float kD = (1.f - kS) * (1.f - metallic);

    Out.vSpecular = kS * pow(max(dot(-vReflect, vLook), 0.f), 5.f);

    Out.vSpecular *= hbao;

    return Out;
}

//����
//PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
//{
//    PS_OUT_LIGHT      Out = (PS_OUT_LIGHT)0;
//
//    vector   vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
//    vector   vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
//    vector   vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
//    vector  vWorldPos;
//
//    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
//    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
//    vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
//    vWorldPos.w = 1.f;
//
//    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);
//
//    /* �佺���̽� ���� ��ġ�� ���Ѵ�. */
//    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
//
//    /* ���彺���̽� ���� ��ġ�� ���Ѵ�. */
//    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
//
//
//    vector      vLightDir = vWorldPos - g_vLightPos;
//    float      fDistance = length(vLightDir);
//
//    float      fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;
//
//    Out.vShade = g_vLightDiffuse *
//        saturate(max(dot(normalize(vLightDir) * -1.f, normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
//
//
//    /* vWorldPos:ȭ�鿡 �׷����� �ִ� �ȼ����� ���� ���� ��ġ�� �޾ƿ;��ϳ�.  */
//    /* 1. ����Ÿ�ٿ� ��ü���� �׸��� �ȼ��� ������ġ�� �����ϴ� ���.(���� �������̶�� ���Ƕ����� ������ �����) */
//    /* 2. ���� ��Ȳ���� �ȼ��� ������ġ(x, y)���� �������ϴ� �۾��� ���� -> z�� ���⶧���� ��������� ����ȯ�� �����.-> ���� z( 0 ~ 1), ViewSpace`s Pixel`s Z��(near ~ far) �޾ƿ���.(�������̾ƴϴ�.)  */
//    vector   vReflect = reflect(normalize(vLightDir), normalize(vNormal));
//    vector   vLook = vWorldPos - g_vCamPosition;
//
//    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f) * fAtt;
//
//    return Out;
//}

PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);

    /* �佺���̽� ���� ��ġ�� ���Ѵ�. */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

    /* ���彺���̽� ���� ��ġ�� ���Ѵ�. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);

    float fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;

    // ��� ���� ����ȭ
    float3 normal = normalize(vNormal.xyz);

    // ���� ���� ���� ����ȭ
    float3 lightDir = normalize(vLightDir);

    // �ں��Ʈ�� ��ǻ�� ���� ���
    float3 lightAmbient = g_vLightAmbient * g_vMtrlAmbient;
    float3 lightDiffuse = g_vLightDiffuse * saturate(max(dot(-lightDir, normal), 0.f));

    // ����ŧ�� ���
    vector vReflect = float4(reflect(-lightDir, normal), 1.f);
    vector vLook = normalize(vWorldPos - g_vCamPosition);

    // Roughness�� Metallic �ؽ�ó ���ø�
    vector vSpecularMap = g_SpecularMapTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
    vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    float roughness = vRoughness.r; // �����Ͻ� ��

    // PBR ���
    float NdotV = max(dot(normal, vLook), 0.f);
    float NdotL = max(dot(normal, lightDir), 0.f);
    float a = roughness * roughness;
    float a2 = a * a;

    float D = a2 / (3.14f * pow(NdotL * NdotL * (a2 - 1.f) + 1.f, 2.f));
    float F = pow(1.f - max(dot(vReflect, vLook), 0.f), 5.f);
    float G = min(1.f, min((2.f * NdotV * NdotL) / (NdotV + NdotL), (2.f * NdotL * (1.f - roughness)) / (NdotV * NdotL)));

    float specular = (D * F * G) / (4.f * NdotV * NdotL + 0.001f);

    float metallic = vMetalic.r;

    float kS = metallic * specular + (1.f - metallic) * 0.04f;
    float kD = (1.f - kS) * (1.f - metallic);

    Out.vShade = float4((lightDiffuse * kD + lightAmbient) * fAtt, 1.f);
    Out.vSpecular = kS * pow(max(dot(-vReflect, vLook), 0.f), 5.f) * fAtt;

    return Out;
}

//SPOTLIGHT ���� ���� �ڵ�
//PS_OUT_LIGHT PS_MAIN_LIGHT_SPOTLIGHT(PS_IN In)
//{
//    PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;
//    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
//    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
//    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
//    vector vWorldPos;
//
//    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
//    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
//    vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
//    vWorldPos.w = 1.f;
//
//    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);
//    /* �佺���̽� ���� ��ġ�� ���Ѵ�. */
//    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
//    /* ���彺���̽� ���� ��ġ�� ���Ѵ�. */
//    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
//    vector vLightDir = vWorldPos - g_vLightPos;
//
//
//    float fDistance = length(vLightDir);
//    vLightDir = normalize(vLightDir);
//    float fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;
//    float fSpotAngle = dot(vLightDir, g_vLightDir);
//
//    if (fSpotAngle > cos(g_fOuterAngle))
//    {
//        float fSpotAtt = saturate((fSpotAngle - cos(g_fOuterAngle)) / (cos(g_fInnerAngle) - cos(g_fOuterAngle)));
//        fAtt *= fSpotAtt; // ���̶��� ���谪
//
//        // ����Ʈ ����� �븻 ������ ���� ���
//        float ndotl = dot(vLightDir * -1.f, normalize(vNormal));
//        // ���� ���� 0�� 1 ���̷� ����
//        ndotl = saturate(ndotl);
//
//        // ���� ���� ���� ���
//        Out.vShade = (g_vLightDiffuse * ndotl + g_vLightAmbient * g_vMtrlAmbient) * fAtt;
//        vector vReflect = reflect(vLightDir, normalize(vNormal));
//        vector vLook = normalize(vWorldPos - g_vCamPosition);
//        Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(vReflect * -1.f, vLook), 0.f), 30.f) * fAtt;
//    }
//
//    return Out;
//}

PS_OUT_LIGHT PS_MAIN_LIGHT_SPOTLIGHT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;

    // �븻 �ؽ�ó ���ø�
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);

    // ���� �ؽ�ó ���ø�
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vWorldPos;

    // ȭ�� ���� ��ǥ�� NDC�� ��ȯ
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; // 0 ~ 1
    vWorldPos.w = 1.f;

    // ���� ���� ����Ͽ� ���� ���� ��ġ�� ���
    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv); // �� �����̽� ���� ��ġ�� ����
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv); // ���� �����̽� ���� ��ġ�� ����

    // ����Ʈ ���� ���� ���
    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    vLightDir = normalize(vLightDir);

    // ���谪 ���
    float fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;
    float fSpotAngle = dot(vLightDir, g_vLightDir);

    if (fSpotAngle > cos(g_fOuterAngle))
    {
        float fSpotAtt = saturate((fSpotAngle - cos(g_fOuterAngle)) / (cos(g_fInnerAngle) - cos(g_fOuterAngle)));
        fAtt *= fSpotAtt; // ���̶��� ���谪

        // �븻 ���� ����ȭ
        float3 normal = normalize(vNormal.xyz);

        // ����Ʈ ����� �븻 ������ ���� ���
        float ndotl = dot(-vLightDir, normal);
        ndotl = saturate(ndotl);

        // �ں��Ʈ�� ��ǻ�� ���� ���
        float3 lightAmbient = g_vLightAmbient * g_vMtrlAmbient;
        float3 lightDiffuse = g_vLightDiffuse * ndotl;

        // ����ŧ�� ���
        vector vReflect = reflect(-vLightDir, float4(normal, 1.f));
        vector vLook = normalize(vWorldPos - g_vCamPosition);

        // Roughness�� Metallic �ؽ�ó ���ø�
        vector vSpecularMap = g_SpecularMapTexture.Sample(LinearSampler, In.vTexcoord);
        vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
        vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
        float roughness = vRoughness.r; // �����Ͻ� ��

        // PBR ���
        float NdotV = max(dot(normal, vLook), 0.f);
        float NdotL = max(dot(normal, -vLightDir), 0.f);
        float a = roughness * roughness;
        float a2 = a * a;

        float D = a2 / (3.14f * pow(NdotL * NdotL * (a2 - 1.f) + 1.f, 2.f));
        float F = pow(1.f - max(dot(vReflect, vLook), 0.f), 5.f);
        float G = min(1.f, min((2.f * NdotV * NdotL) / (NdotV + NdotL), (2.f * NdotL * (1.f - roughness)) / (NdotV * NdotL)));

        float specular = (D * F * G) / (4.f * NdotV * NdotL + 0.001f);

        float metallic = vMetalic.r;

        float kS = metallic * specular + (1.f - metallic) * 0.04f;
        float kD = (1.f - kS) * (1.f - metallic);

        // ���� ���� ���� ���
        Out.vShade = float4((lightDiffuse * kD + lightAmbient) * fAtt, 1.f);
        Out.vSpecular = kS * pow(max(dot(-vReflect, vLook), 0.f), 5.f) * fAtt;
    }

    return Out;
}
float hash(float3 p)
{
    p = frac(p * 0.3183099 + .1);
    p *= 17.0;
    return frac(p.x * p.y * p.z * (p.x + p.y + p.z));
}

float noise(float3 x)
{
    float3 i = floor(x);
    float3 f = frac(x);
    f = f * f * (3.0 - 2.0 * f);

    return lerp(lerp(lerp(hash(i + float3(0, 0, 0)),
        hash(i + float3(1, 0, 0)), f.x),
        lerp(hash(i + float3(0, 1, 0)),
            hash(i + float3(1, 1, 0)), f.x), f.y),
        lerp(lerp(hash(i + float3(0, 0, 1)),
            hash(i + float3(1, 0, 1)), f.x),
            lerp(hash(i + float3(0, 1, 1)),
                hash(i + float3(1, 1, 1)), f.x), f.y), f.z);
}

float fbm(float3 p)
{
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    for (int i = 0; i < 6; i++)
    {
        value += amplitude * noise(p * frequency);
        amplitude *= 0.5;
        frequency *= 2.0;
    }
    return value;
}



float2 CalculateScreenSpacePosition(float3 worldPosition, matrix viewProjMatrix)
{
    // ���� ��ǥ�� ��ũ�� ��ǥ�� ��ȯ
    float4 screenPosition = mul(float4(worldPosition, 1.0f), viewProjMatrix);
    screenPosition /= screenPosition.w;

    // ��ũ�� ��ǥ�� 0 ~ 1 ���̷� ����ȭ
    float2 screenSpacePosition = screenPosition.xy * 0.5f + 0.5f;

    return screenSpacePosition;
}

float4 RayMarchingGodRays(float2 texCoord, float2 lightScreenPos, float3 lightColor, Texture2D depthTex, SamplerState linearSampler)
{
    const int NUM_SAMPLES = 32;
    const float DECAY = 0.95;
    const float DENSITY = 0.8;
    const float WEIGHT = 0.5;
    const float EXPOSURE = 0.3;

    float2 deltaTexCoord = normalize(lightScreenPos - texCoord) * DENSITY / NUM_SAMPLES;
    float3 color = float3(0, 0, 0);
    float illuminationDecay = 1.0;

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        texCoord += deltaTexCoord;
        float samplDepth = depthTex.Sample(linearSampler, texCoord).r;
        if (samplDepth < 1.0)
        {
            float3 sampleColor = lightColor;
            sampleColor *= illuminationDecay * WEIGHT;
            color += sampleColor;
            illuminationDecay *= DECAY;
        }

    }

    return float4(color * EXPOSURE, 1.0);

}


PS_OUT PS_MAIN_DEFERRED_RESULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDecal = g_EffectTexture.Sample(LinearSampler, In.vTexcoord);
    vector vShadow = g_ShadowTexture.Sample(LinearSampler, In.vTexcoord);
    if (0.0f == vDiffuse.a)
        discard;

    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

    vector vMetalicDesc = g_MetalicTexture.Sample(PointSampler, In.vTexcoord);

    float fDecalDepth = g_DecalDepthTexture.Sample(LinearSampler, In.vTexcoord).x;
    vector vColor;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);

    if (fDecalDepth <= vDepthDesc.x)
    {
        vColor = lerp(vDiffuse, vDecal, vDecal.a) * vShade + vSpecular * lerp(float4(1.f, 1.f, 1.f, 1.f), vDiffuse, vMetalicDesc);
    }
    else
    {
        vColor = vDiffuse * vShade + vSpecular * lerp(float4(1.f, 1.f, 1.f, 1.f), vDiffuse, vMetalicDesc);
    }
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);

    /* �佺���̽� ���� ��ġ�� ���Ѵ�. */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

    /* ���彺���̽� ���� ��ġ�� ���Ѵ�. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    if (vShadow.r != 0.f)
    {
        vShadow.a = vShadow.r;
        vShadow.rgb = float3(0.f, 0.f, 0.f);
        vColor = lerp(vColor, (vShadow + vColor) * 0.5f, vShadow.a);
    }

    float HBAO = g_HBAOTexture.Sample(LinearSampler, In.vTexcoord).r;

    vColor *= HBAO;


    Out.vColor = vColor;

    /*  float2 lightScreenPos = CalculateScreenSpacePosition(g_vLightPos.xyz, Test_g_LightViewMatrix * Test_g_LightProjMatrix);

      float3 lightColor = g_vLightDiffuse.xyz;
      float4 godrays = RayMarchingGodRays(In.vTexcoord, lightScreenPos, lightColor, g_DepthTexture, LinearSampler);

      Out.vColor.rgb += godrays.rgb;*/


      //�Ȱ�
    float3 cameraToWorldPos = vWorldPos.xyz - g_vCamPosition.xyz;
    float distanceToCamera = length(cameraToWorldPos);

    // �Ȱ� ���� ���
    float3 fogDirection = normalize(cameraToWorldPos);

    // ������ ���� ���� ���� (�Ȱ� ������ ����)
    float noiseRange = g_fFogRange * 0.5f;

    // ù ��° �Ȱ� ���
    float3 timeOffset1 = float3(g_Time * g_fFogTimeOffset * 1.2f, g_Time * g_fFogTimeOffset, g_Time * g_fFogTimeOffset * 0.7f);
    float3 fbmCoord1 = vWorldPos.xyz * g_fNoiseSize + timeOffset1;
    float fbmValue1 = fbm(fbmCoord1);

    // �� ��° �Ȱ� ���
    float3 timeOffset2 = float3(-g_Time * g_fFogTimeOffset2 * 0.8f, -g_Time * g_fFogTimeOffset2 * 0.6f, -g_Time * g_fFogTimeOffset2 * 0.9f);
    float3 fbmCoord2 = vWorldPos.xyz * g_fNoiseSize2 + timeOffset2;
    float fbmValue2 = fbm(fbmCoord2);

    // ������ ���� ��� �� �Ÿ��� ���� ����
    float noiseFade = saturate(distanceToCamera / g_fFogRange);
    float noiseSample1 = lerp(1.0, 1.0 + (fbmValue1 - 0.5) * 2.0 * g_fNoiseIntensity, noiseFade);
    float noiseSample2 = lerp(1.0, 1.0 + (fbmValue2 - 0.5) * 2.0 * g_fNoiseIntensity2, noiseFade);

    // �Ÿ� ��� �Ȱ� ���
    float baseFog = saturate(distanceToCamera / g_fFogRange);
    float distanceFog1 = baseFog * noiseSample1;
    float distanceFog2 = baseFog * noiseSample2;

    //float fogFactor1 = (1.0 - exp(-distanceToCamera * fogDirection.y * g_fFogHeightFalloff)) / (fogDirection.y + 0.001);
    //float fogFactor2 = (1.0 - exp(-distanceToCamera * fogDirection.y * g_fFogHeightFalloff)) / (fogDirection.y + 0.001);

    //fogFactor1 = saturate(fogFactor1);
    //fogFactor2 = saturate(fogFactor2);

    // �Ÿ� ��� �Ȱ��� ���� ��� �Ȱ� ����
    float combinedFog1 = distanceFog1 * g_fFogGlobalDensity;
    float combinedFog2 = distanceFog2 * g_fFogGlobalDensity;

    // ���� �Ȱ� ���� ���
    float finalFogFactor1 = saturate(combinedFog1);
    float finalFogFactor2 = saturate(combinedFog2);

    // ���� ���� ���
    float4 finalColor = lerp(vColor, g_vFogColor, finalFogFactor1);
    finalColor = lerp(finalColor, g_vFogColor2, finalFogFactor2 * g_fFogBlendFactor);

    vector vEmissiveDesc = g_EmissiveTexture.Sample(PointSampler, In.vTexcoord);
    finalColor.rgb += vEmissiveDesc.rgb;

    Out.vColor = finalColor;
    return Out;
}

PS_OUT PS_SHADOW(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    // ���� �ؽ�ó���� ���ø�
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);

    // Ŭ�� ���� ��ǥ ���
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; // ���� �� (0 ~ 1)
    vWorldPos.w = 1.f;

    // ���� ���� ���� �������� ��ȯ
    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);

    // �� �������� ��ȯ
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

    // ���� �������� ��ȯ
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);


    // �׸��� �� �������� ��ȯ
    vector vLightPos = mul(vWorldPos, g_LightViewMatrix);
    vLightPos = mul(vLightPos, g_LightProjMatrix);

    float2 vTexcoord;
    vTexcoord.x = vLightPos.x / vLightPos.w * 0.5f + 0.5f;
    vTexcoord.y = vLightPos.y / vLightPos.w * -0.5f + 0.5f;

    float lightDepthDesc = g_LightDepthTexture.Sample(LinearSampler, vTexcoord).r;

    float fLightOldDepth = lightDepthDesc * 3000.f;

    //if (fLightOldDepth + 0.001f < vLightPos.z)
    //    Out.vColor = vector(1.f, 1.f, 1.f, 1.f);
    if (fLightOldDepth + g_fShadowThreshold < vLightPos.w)
        Out.vColor = vector(1.f, 1.f, 1.f, 1.f);

    return Out;
}

PS_OUT PS_SHADOW_RESULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vShadow_Move = g_Shadow_MoveTexture.Sample(LinearSampler, In.vTexcoord);
    vector vShadow_NotMove = g_Shadow_NotMoveTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = vShadow_Move + vShadow_NotMove;

    return Out;
}

PS_OUT PS_DISTORTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float2 objectCenter = float2(0.5f, 0.5f); // ��ü �߽��� �ؽ�ó ��ǥ (ȭ�� �߽�)
    float waveFrequency = 1.f; // �ĵ� ���ļ�
    float waveAmplitude = 0.005f; // �ĵ� ����
    float distortionRadius = 0.f; // �ϱ׷��� �ݰ�

    if (g_EffectTexture.Sample(LinearSampler, In.vTexcoord).a == 0.f) return Out;

    // ��ü �߽����κ����� �Ÿ� ���
    float2 distance = In.vTexcoord - objectCenter;
    float dist = length(distance);

    // �ϱ׷��� ȿ�� ����
    float2 distortedTex = In.vTexcoord;
    //if (dist < distortionRadius)
    //{
        // �ĵ� ȿ�� ����: ���� �Լ��� ����Ͽ� �ؽ�ó ��ǥ�� ����
    distortedTex = float2(
        In.vTexcoord.x + sin(dist * waveFrequency + g_DistortionTexture.Sample(LinearSampler, float2(In.vTexcoord.x /*+ g_Time*/, In.vTexcoord.y /*+ g_Time*/)).r) * waveAmplitude,
        In.vTexcoord.y + sin(dist * waveFrequency + g_DistortionTexture.Sample(LinearSampler, float2(In.vTexcoord.x /*+ g_Time*/, In.vTexcoord.y /*+ g_Time*/)).r) * waveAmplitude
    );
    //}

    // ������ �ؽ�ó ��ǥ�� �⺻ �ؽ�ó ���ø�
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, distortedTex);
    // ����� ������ �� �� ��Ȯ�� ���̵��� �� ����
    Out.vColor.rgb += float3(0.02f, 0.02f, 0.02f);
    Out.vColor.a = 1.f;

    return Out;
}

PS_OUT PS_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    // ȭ�� �������� ����
    float value = 0.4f;
    if (In.vTexcoord.x + In.vTexcoord.y < 1.f)
    {
        In.vTexcoord.x = In.vTexcoord.x - g_fBRIS * value;
        In.vTexcoord.y = In.vTexcoord.y + g_fBRIS * value;
    }
    else
    {
        In.vTexcoord.x = In.vTexcoord.x + g_fBRIS * value;
        In.vTexcoord.y = In.vTexcoord.y - g_fBRIS * value;
    }

    float4 maskColor = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);


    // ȭ�� ������ ����
    value = 1.f;
    if (In.vTexcoord.x + In.vTexcoord.y < 1.f)
    {
        In.vTexcoord.x = In.vTexcoord.x - (maskColor.g + maskColor.b) * g_fMirror * value;
        In.vTexcoord.y = In.vTexcoord.y + (maskColor.g + maskColor.b) * g_fMirror * value;
    }
    else
    {
        In.vTexcoord.x = In.vTexcoord.x + (maskColor.g + maskColor.b) * g_fMirror * value;
        In.vTexcoord.y = In.vTexcoord.y - (maskColor.g + maskColor.b) * g_fMirror * value;
    }

    Out.vColor = g_ResultTexture.Sample(LinearSampler, In.vTexcoord) + g_GodRayTexture.Sample(LinearSampler, In.vTexcoord);
    float2 velocity = g_EffectTexture.Sample(LinearSampler, In.vTexcoord).xy;

    const int NumSamples = 8;

    for (int i = 0; i < NumSamples; ++i)
    {
        float2 offset = velocity * (float(i) / float(NumSamples - 1) - 0.5f);
        Out.vColor += g_ResultTexture.Sample(LinearSampler, saturate(In.vTexcoord + offset));
    }

    Out.vColor /= float(NumSamples);

    if (g_fMirror != 0.f)
    {
        Out.vColor = float4(1.f - Out.vColor.r, 1.f - Out.vColor.g, 1.f - Out.vColor.b, 1.f);
    }
    else if (g_fBRIS > 0.09f)
    {
        Out.vColor = float4(lerp(Out.vColor.r, 1.f - Out.vColor.r, (g_fBRIS - 0.09f) * 18.f),
            lerp(Out.vColor.g, 1.f - Out.vColor.g, (g_fBRIS - 0.09f) * 18.f),
            lerp(Out.vColor.b, 1.f - Out.vColor.b, (g_fBRIS - 0.09f) * 18.f),
            1.f);
    }

    return Out;
}

PS_OUT PS_FINAL2(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    // ȭ�� �������� ����
    float value = 0.4f;
    if (In.vTexcoord.x + In.vTexcoord.y < 1.f)
    {
        In.vTexcoord.x = In.vTexcoord.x - g_fBRIS * value;
        In.vTexcoord.y = In.vTexcoord.y + g_fBRIS * value;
    }
    else
    {
        In.vTexcoord.x = In.vTexcoord.x + g_fBRIS * value;
        In.vTexcoord.y = In.vTexcoord.y - g_fBRIS * value;
    }

    float4 maskColor = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);


    // ȭ�� ������ ����
    value = 1.f;
    if (In.vTexcoord.x + In.vTexcoord.y < 1.f)
    {
        In.vTexcoord.x = In.vTexcoord.x - (maskColor.g + maskColor.b) * g_fMirror * value;
        In.vTexcoord.y = In.vTexcoord.y + (maskColor.g + maskColor.b) * g_fMirror * value;
    }
    else
    {
        In.vTexcoord.x = In.vTexcoord.x + (maskColor.g + maskColor.b) * g_fMirror * value;
        In.vTexcoord.y = In.vTexcoord.y - (maskColor.g + maskColor.b) * g_fMirror * value;
    }

    Out.vColor = g_ResultTexture.Sample(LinearSampler, In.vTexcoord) + g_GodRayTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = pow(Out.vColor, g_Value);

    Out.vColor.rgb /= (Out.vColor.rgb + 1.f);
    Out.vColor = pow(Out.vColor, 1 / g_Value);

    float2 velocity = g_EffectTexture.Sample(LinearSampler, In.vTexcoord).xy;

    const int NumSamples = 8;

    for (int i = 0; i < NumSamples; ++i)
    {
        float2 offset = velocity * (float(i) / float(NumSamples - 1) - 0.5f);
        Out.vColor += g_ResultTexture.Sample(LinearSampler, saturate(In.vTexcoord + offset));
    }

    Out.vColor /= float(NumSamples);

    if (g_fMirror != 0.f)
    {
        Out.vColor = float4(1.f - Out.vColor.r, 1.f - Out.vColor.g, 1.f - Out.vColor.b, 1.f);
    }
    else if (g_fBRIS > 0.09f)
    {
        Out.vColor = float4(lerp(Out.vColor.r, 1.f - Out.vColor.r, (g_fBRIS - 0.09f) * 18.f),
            lerp(Out.vColor.g, 1.f - Out.vColor.g, (g_fBRIS - 0.09f) * 18.f),
            lerp(Out.vColor.b, 1.f - Out.vColor.b, (g_fBRIS - 0.09f) * 18.f),
            1.f);
    }

    return Out;
}

PS_OUT PS_FINAL3(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    // ȭ�� �������� ����
    float value = 0.4f;
    if (In.vTexcoord.x + In.vTexcoord.y < 1.f)
    {
        In.vTexcoord.x = In.vTexcoord.x - g_fBRIS * value;
        In.vTexcoord.y = In.vTexcoord.y + g_fBRIS * value;
    }
    else
    {
        In.vTexcoord.x = In.vTexcoord.x + g_fBRIS * value;
        In.vTexcoord.y = In.vTexcoord.y - g_fBRIS * value;
    }

    float4 maskColor = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);


    // ȭ�� ������ ����
    value = 1.f;
    if (In.vTexcoord.x + In.vTexcoord.y < 1.f)
    {
        In.vTexcoord.x = In.vTexcoord.x + (maskColor.g + maskColor.b) * g_fMirror * value;
        In.vTexcoord.y = In.vTexcoord.y + (maskColor.g + maskColor.b) * g_fMirror * value;
    }
    else
    {
        In.vTexcoord.x = In.vTexcoord.x - (maskColor.g + maskColor.b) * g_fMirror * value;
        In.vTexcoord.y = In.vTexcoord.y - (maskColor.g + maskColor.b) * g_fMirror * value;
    }

    if (In.vTexcoord.x > 1.f)
    {
        In.vTexcoord.x = 2.f - In.vTexcoord.x;
    }
    if (In.vTexcoord.y > 1.f)
    {
        In.vTexcoord.y = 2.f - In.vTexcoord.y;
    }
    if (In.vTexcoord.x < 0.f)
    {
        In.vTexcoord.x *= -1.f;
    }
    if (In.vTexcoord.y < 0.f)
    {
        In.vTexcoord.y *= -1.f;
    }

    Out.vColor = g_ResultTexture.Sample(LinearSampler, In.vTexcoord) + g_GodRayTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = pow(Out.vColor, g_Value);

    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;

    Out.vColor = saturate((Out.vColor * (a * Out.vColor + b)) / (Out.vColor * (c * Out.vColor + d) + e));
    Out.vColor = pow(Out.vColor, 1 / g_Value);
    Out.vColor.a = 1.f;

    float2 velocity = g_EffectTexture.Sample(LinearSampler, In.vTexcoord).xy;

    const int NumSamples = 8;

    for (int i = 0; i < NumSamples; ++i)
    {
        float2 offset = velocity * (float(i) / float(NumSamples - 1) - 0.5f);
        Out.vColor += g_ResultTexture.Sample(LinearSampler, saturate(In.vTexcoord + offset));
    }

    Out.vColor /= float(NumSamples);

    if (g_fMirror != 0.f)
    {
        Out.vColor = float4(1.f - Out.vColor.r, 1.f - Out.vColor.g, 1.f - Out.vColor.b, 1.f);
    }
    else if (g_fBRIS > 0.09f)
    {
        Out.vColor = float4(lerp(Out.vColor.r, 1.f - Out.vColor.r, (g_fBRIS - 0.09f) * 18.f),
            lerp(Out.vColor.g, 1.f - Out.vColor.g, (g_fBRIS - 0.09f) * 18.f),
            lerp(Out.vColor.b, 1.f - Out.vColor.b, (g_fBRIS - 0.09f) * 18.f),
            1.f);
    }

    return Out;
}

PS_OUT PS_LUT(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vResult = g_ResultTexture.Sample(LinearSampler, In.vTexcoord);
    vector vBlur = g_BlurTexture.Sample(LinearSampler, In.vTexcoord);
    vResult = (vResult + vBlur) * 0.5f;

    vector vDistortion = g_DistortionTexture.Sample(LinearSampler, In.vTexcoord);

    if (vDistortion.a > 0.f) Out.vColor = vDistortion;
    else Out.vColor = vResult;

    float2 LutSize = float2(1.0 / 256.0, 1.0 / 16.0);
    float4 LutUV;
    Out.vColor = saturate(Out.vColor) * 15.0;
    LutUV.w = floor(Out.vColor.b);
    LutUV.xy = (Out.vColor.rg + 0.5) * LutSize;
    LutUV.x += LutUV.w * LutSize.y;
    LutUV.z = LutUV.x + LutSize.y;
    Out.vColor = lerp(g_LUTTexture.SampleLevel(LinearSampler, LutUV.xy, 0), g_LUTTexture.SampleLevel(LinearSampler, LutUV.zy, 0), Out.vColor.b - LutUV.w);
    Out.vColor.a = 1.f;

    return Out;
}

PS_OUT PS_NOLUT(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vResult = g_ResultTexture.Sample(LinearSampler, In.vTexcoord);
    vector vBlur = g_BlurTexture.Sample(LinearSampler, In.vTexcoord);
    vResult = vResult + vBlur;

    vector vDistortion = g_DistortionTexture.Sample(LinearSampler, In.vTexcoord);

    if (vDistortion.a > 0.f) Out.vColor = vDistortion;
    else Out.vColor = vResult;

    return Out;
}

PS_OUT PS_DOWNSAMPLE4X4(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    //Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            Out.vColor += g_DiffuseTexture.Sample(LinearSampler, saturate(In.vTexcoord + float2(-1.5f + i, -1.5f + j) / float2(g_fTexW, g_fTexH)));
        }
    }
    Out.vColor /= 16.f;
    return Out;
}

PS_OUT PS_DOWNSAMPLE5X5(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    //Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            Out.vColor += g_DiffuseTexture.Sample(LinearSampler, saturate(In.vTexcoord + float2(-2.f + i, -2.f + j) / float2(g_fTexW, g_fTexH)));
        }
    }
    Out.vColor /= 25.f;
    return Out;
}

PS_OUT PS_UPSAMPLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vOriginalColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDownSampledColor = g_EffectTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = (vOriginalColor + vDownSampledColor * 0.5f);

    return Out;
}

PS_OUT PS_BLURX(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float2 vUV = (float2)0;

    if (g_BlurNum == 0)
    {
        for (int i = -6; i < 7; ++i)
        {
            vUV = In.vTexcoord + float2(1.f / g_fTexW * i, 0);
            if (vUV.x >= 0.f && vUV.x <= 1.f && vUV.y >= 0.f && vUV.y <= 1.f)
                Out.vColor += g_EffectTexture.Sample(LinearSampler, vUV);
        }

        Out.vColor.rgb /= 13.f;
    }
    else if (g_BlurNum == 1)
    {
        for (int i = -6; i < 7; ++i)
        {
            vUV = In.vTexcoord + float2(1.f / g_fTexW * i, 0);
            if (vUV.x >= 0.f && vUV.x <= 1.f && vUV.y >= 0.f && vUV.y <= 1.f)
                Out.vColor += g_fWeight[6 + i] * g_EffectTexture.Sample(LinearSampler, vUV);
        }

        Out.vColor /= g_fTotal;
    }
    // ����Ƚ�� 1/2 ����
    else if (g_BlurNum == 2)
    {
        for (int i = -13; i < 13; ++i)
        {
            vUV = In.vTexcoord + float2(1.f / g_fTexW * i * 2.f, 0);
            if (vUV.x >= 0.f && vUV.x <= 1.f && vUV.y >= 0.f && vUV.y <= 1.f)
                Out.vColor += g_fWeight2[26 + i * 2] * g_EffectTexture.Sample(LinearSampler, vUV);
        }

        Out.vColor /= g_fTotal * 0.8f;
    }
    // ����Ƚ�� 1/10 ����
    else if (g_BlurNum == 3)
    {
        for (int i = -13; i < 13; ++i)
        {
            vUV = In.vTexcoord + float2(1.f / g_fTexW * i * 10.f, 0);
            if (vUV.x >= 0.f && vUV.x <= 1.f && vUV.y >= 0.f && vUV.y <= 1.f)
                Out.vColor += g_fWeight3[130 + i * 10] * g_EffectTexture.Sample(LinearSampler, vUV);
        }

        Out.vColor /= g_fTotal * 0.8f;
    }
    //Out.vColor = g_EffectTexture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

PS_OUT PS_BLURY(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float2 vUV = (float2)0;

    if (g_BlurNum == 0)
    {
        for (int i = -6; i < 7; ++i)
        {
            vUV = In.vTexcoord + float2(0, 1.f / g_fTexH * i);
            if (vUV.x >= 0.f && vUV.x <= 1.f && vUV.y >= 0.f && vUV.y <= 1.f)
                Out.vColor += g_EffectTexture.Sample(LinearSampler, vUV);
        }

        Out.vColor.rgb /= 13.f;
    }
    else if (g_BlurNum == 1)
    {
        for (int i = -6; i < 7; ++i)
        {
            vUV = In.vTexcoord + float2(0, 1.f / g_fTexH * i);
            if (vUV.x >= 0.f && vUV.x <= 1.f && vUV.y >= 0.f && vUV.y <= 1.f)
                Out.vColor += g_fWeight[6 + i] * g_EffectTexture.Sample(LinearSampler, vUV);
        }

        Out.vColor /= 3;
    }
    else if (g_BlurNum == 2)
    {
        for (int i = -13; i < 13; ++i)
        {
            vUV = In.vTexcoord + float2(0, 1.f / g_fTexH * i * 2.f);
            if (vUV.x >= 0.f && vUV.x <= 1.f && vUV.y >= 0.f && vUV.y <= 1.f)
                Out.vColor += g_fWeight2[26 + i * 2.f] * g_EffectTexture.Sample(LinearSampler, vUV);
        }

        Out.vColor /= g_fTotal * 0.8f;
    }
    else if (g_BlurNum == 3)
    {
        for (int i = -13; i < 13; ++i)
        {
            vUV = In.vTexcoord + float2(0, 1.f / g_fTexH * i * 10.f);
            if (vUV.x >= 0.f && vUV.x <= 1.f && vUV.y >= 0.f && vUV.y <= 1.f)
                Out.vColor += g_fWeight3[130 + i * 10.f] * g_EffectTexture.Sample(LinearSampler, vUV);
        }

        Out.vColor /= g_fTotal * 0.8f;
    }
    //Out.vColor = g_EffectTexture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

PS_OUT PS_BLOOM(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    //float4 color = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    //float brightness = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));

    //if (brightness > 0.8f)
    //   Out.vColor = color;
    //else
    //   Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

struct PS_OUT_DECAL
{
    vector      vColor : SV_TARGET0;
    vector      vDepth : SV_TARGET1;
};

PS_OUT_DECAL PS_DECAL(PS_IN In)
{
    PS_OUT_DECAL Out = (PS_OUT_DECAL)0;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    Out.vDepth = vDepthDesc;
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; /* 0 ~ 1 */
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);

    /* �佺���̽� ���� ��ġ�� ���Ѵ�. */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

    /* ���彺���̽� ���� ��ġ�� ���Ѵ�. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    // ���� �������� ��Į �������� ��ȯ�մϴ�.
    float4 vDecalSpacePos = mul(vWorldPos, g_WorldMatrixInv);

    if (abs(vDecalSpacePos.x) <= 1.f && abs(vDecalSpacePos.y) <= 1.f && abs(vDecalSpacePos.z) <= 1.f)
    {
        // �븻 ���͸� �����ɴϴ�.
        float3 vNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord).xyz * 2.0 - 1.0;

        // ��Į�� ���� ���� ����մϴ�.
        float3 vDecalForward = normalize(g_WorldMatrixInv[2].xyz);
        float3 vDecalRight = normalize(g_WorldMatrixInv[0].xyz);
        float3 vDecalUp = normalize(g_WorldMatrixInv[1].xyz);

        // UV ��ǥ�� ����Ѵ�.
        float2 vDecalTexcoord;
        if (abs(dot(vNormal, vDecalForward)) > 0.707) // cos(45��) 0.707
        {
            // ���� �Ǵ� �ĸ�
            vDecalTexcoord.x = vDecalSpacePos.x * 0.5 + 0.5;
            vDecalTexcoord.y = vDecalSpacePos.y * -0.5 + 0.5;
        }
        else if (abs(dot(vNormal, vDecalUp)) > 0.707)
        {
            // ��� �Ǵ� �ϴ�
            vDecalTexcoord.x = vDecalSpacePos.x * 0.5 + 0.5;
            vDecalTexcoord.y = vDecalSpacePos.z * -0.5 + 0.5;
        }
        else
        {
            // ����
            vDecalTexcoord.x = vDecalSpacePos.y * -0.5 + 0.5;
            vDecalTexcoord.y = vDecalSpacePos.z * -0.5 + 0.5;
        }

        float4 vDecalColor = g_EffectTexture.Sample(LinearSampler, vDecalTexcoord);
        if (vDecalColor.a <= 0.1f)
        {
            discard;
        }

        if (abs(dot(vNormal, vDecalForward)) > 0.707) // cos(45��) 0.707
        {
            // ���� �Ǵ� �ĸ�
            if (abs(vDecalSpacePos.z) > 0.8f)
            {
                vDecalColor.a = 0.f;
            }
        }
        else if (abs(dot(vNormal, vDecalUp)) > 0.707)
        {
            // ��� �Ǵ� �ϴ�
            if (abs(vDecalSpacePos.y) > 0.8f)
            {
                vDecalColor.a = 0.f;
            }
        }
        else
        {
            // ����
            if (abs(vDecalSpacePos.x) > 0.8f)
            {
                vDecalColor.a = 0.f;
            }
        }

        Out.vColor = vDecalColor;
    }
    else
    {
        discard;
    }

    return Out;
}

PS_OUT PS_REFLECTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    vector vMirror = g_MirrorTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMirror.b < 0.1f)
    {
        Out.vColor = vDiffuse;
        return Out;
    }

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);

    // ���� ��ǥ ���
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    // �븻 ���� ��� (�븻 ���� ���� ����)
    float3 normal = normalize(lerp(float3(0, 0, 1), vNormalDesc.xyz * 2.f - 1.f, 0.3));

    // �� ���� ���
    float3 viewDir = normalize(g_vCamPosition.xyz - vWorldPos.xyz);

    // ������ ������ ȿ�� ���
    float NdotV = saturate(dot(normal, viewDir));
    float fresnel = pow(1 - NdotV, g_fFresnelPower);

    // Caustic ����� �̿��� �ĵ� ȿ�� ��� (���� ����)
    float2 causticCoord = In.vTexcoord * g_fWaveFrequency + float2(g_Time * g_fWaveTimeOffset * 2.f , g_Time * g_fWaveTimeOffset * 2.f);
    vector vCausticNoise = g_CausticTexture.Sample(LinearSampler, causticCoord);
    float2 waveOffset = (vCausticNoise.xy - 0.5) * g_fWaveStrength * 0.1;

    // �븻 ���� ����Ͽ� �ĵ� ���� ���� (���� ����)
    waveOffset *= length(normal.xy) * 0.5f;

    // �ְ�� ��ǥ�� ���÷��� �ؽ�ó ���ø�
    float2 distortedCoord = float2((1.f - In.vTexcoord.x), In.vTexcoord.y) + waveOffset;
    vector vReflection = g_EffectTexture.Sample(LinearSampler, distortedCoord);

    // ������ ȿ���� ������ ���� ���� (�ε巯�� ��ȯ)
    float blendFactor = min(0.1f, saturate(/*fresnel **/ vMirror.b));
   // vReflection *= blendFactor;

    vMirror.b *= vMirror.b;
    Out.vColor = lerp(vDiffuse, vReflection, vMirror.b);
    Out.vColor.a = 1.f;
    return Out;
}

PS_OUT PS_GODRAY(PS_IN In)
{
    PS_OUT		Out = (PS_OUT)0;

    //float4 lightworldPos = float4(86.f, 50.f, -86.f, 1.f);
    float4 lightworldPos = float4(-500.f, 200.f, 0.f, 1.f);
    float4 floorWorldPos = float4(-407.f, 70.f, 0.f, 1.f);

    matrix      matVP = mul(g_GodRayViewMatrix, g_GodRayProjMatrix);

    // ���� �ؽ�ó���� ���ø�
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);

    // Ŭ�� ���� ��ǥ ���
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x; // ���� �� (0 ~ 1)
    vWorldPos.w = 1.f;

    // ���� ���� ���� �������� ��ȯ
    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);

    // �� �������� ��ȯ
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

    // ���� �������� ��ȯ
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    if (vWorldPos.x > -360.f)
    {
        return Out;
    }

    lightworldPos = mul(lightworldPos, matVP);
    lightworldPos.x = lightworldPos.x / lightworldPos.w * 0.5f + 0.5f;
    lightworldPos.y = lightworldPos.y / lightworldPos.w * -0.5f + 0.5f;
    float2 lightPosition = float2(lightworldPos.x, lightworldPos.y);
    floorWorldPos = mul(floorWorldPos, matVP);
    floorWorldPos.x = floorWorldPos.x / floorWorldPos.w * 0.5f + 0.5f;
    floorWorldPos.y = floorWorldPos.y / floorWorldPos.w * -0.5f + 0.5f;
    float2 FloorPosition = float2(floorWorldPos.x, floorWorldPos.y);

    float exposure = 0.3f;       // ���� ��
    float decay = 0.98f;          // ���� ��
    float density = 0.001f;        // �е� ��
    float weight = 0.1f;         // ����ġ ��

    //Ư�� �������θ� ���� �������� ���� ���� ����
    float2 desiredDirection = normalize(float2(FloorPosition.x - lightPosition.x, FloorPosition.y - lightPosition.y));

    // �������κ����� ���� ���� ���
    float2 deltaTexCoord = (In.vTexcoord - lightPosition) * density;
    float fDot = dot(normalize(deltaTexCoord), desiredDirection);
    float fDegree = degrees(acos(fDot));
    
    float2 currentTexCoord = In.vTexcoord;
    float illuminationDecay = 1.0f;
    float4 color = float4(0, 0, 0, 1);
    float initialDepth = g_DepthTexture.Sample(LinearSampler, In.vTexcoord).r;

    // ���� ��Ī ����
    for (int i = 0; i < 1; ++i)
    {
        currentTexCoord -= deltaTexCoord;

        // ��� üũ: ȭ�� ������ ������ �ʵ���
        if (currentTexCoord.x < 0.0f || currentTexCoord.x > 1.0f || currentTexCoord.y < 0.0f || currentTexCoord.y > 1.0f)
            break;

        float sampleDepth = g_DepthTexture.Sample(LinearSampler, currentTexCoord).r;

        // ���� ���� ���Ͽ� ���ٱ� ����
        if (sampleDepth < initialDepth - 0.0005f) // ���ø��� ���̰� �� ����� ��� ����
        {
            break; // ���ٱ� ����
        }

        color += illuminationDecay * weight;
        illuminationDecay *= decay;
    }
    Out.vColor = color * exposure;
    Out.vColor *= saturate(1.f - fDegree / 20.f);
  
    return Out;
}

technique11 DefaultTechnique
{
    /* Ư�� �������� ������ �� �����ؾ��� ���̴� ����� ��Ʈ���� ���̰� �ִ�. */
    pass DebugRender_0
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_Default, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional_1
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
    }

    pass Light_Point_2
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
    }

    pass Deferred_Result_3
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_MAIN_DEFERRED_RESULT();
    }

    pass Distortion_4
    {
       SetRasterizerState(RS_NoCull);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);



       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;


       PixelShader = compile ps_5_0 PS_DISTORTION();
    }

    pass Final_5
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_FINAL();

    }

    pass Light_Spotlight_6
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_MAIN_LIGHT_SPOTLIGHT();
    }

    pass Reinhard_7
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_FINAL2();

    }

    pass ACES_Filmic_8
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_FINAL3();
    }

    pass DownSampling4x4_9
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_DOWNSAMPLE4X4();
    }

    pass DownSampling5x5_10
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_DOWNSAMPLE5X5();
    }

    pass UpSampling_11
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_UPSAMPLE();
    }

    pass BlurX_12
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_BLURX();
    }

    pass BlurY_13
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_BLURY();
    }

    pass Bloom_14
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_BLOOM();
    }

    pass LUT_15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_LUT();
    }

    pass NOLUT_16
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_NOLUT();
    }

    pass Decal_17
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DECAL();
    }

    pass Reflection_18
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_REFLECTION();
    }

    pass Shadow_19
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_SHADOW();
    }

    pass Shadow_Result_20
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_SHADOW_RESULT();
    }

    pass GodRay_21
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


       /* � ���̴��� ��������. ���̴��� �� �������� ����������. �������Լ��� ��������. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_GODRAY();
    }
}