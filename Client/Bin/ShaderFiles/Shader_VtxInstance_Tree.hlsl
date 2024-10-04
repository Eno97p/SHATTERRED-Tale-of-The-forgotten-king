#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix, g_PrevViewMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_SpecularTexture;
texture2D g_OpacityTexture;
texture2D g_EmissiveTexture;
texture2D g_RoughnessTexture;
texture2D g_MetalicTexture;
texture2D g_DisolveTexture;
texture2D g_BlurTexture;

float g_DisolveValue = 1.f;
float g_TexcoordY = 1.f;
float4 g_fColor = { 1.f, 1.f, 1.f, 1.f };

bool g_bDiffuse = false;
bool g_bNormal = false;
bool g_bSpecular = false;
bool g_bOpacity = false;
bool g_bEmissive = false;
bool g_bRoughness = false;
bool g_bMetalic = false;
bool g_MotionBlur = false;

float g_fCamFar = 3000.f;
//바람 시뮬레이션용
float g_fTime;
float3 g_vWindDirection;
float g_fWindStrength;

float3 g_LeafCol;
struct VS_IN
{
    float3      vPosition : POSITION;
    float3      vNormal : NORMAL;
    float2      vTexcoord : TEXCOORD0;
    float3      vTangent : TANGENT;

    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vLocalPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2      vVelocity : TEXCOORD3;

};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;

    matrix TransformMatrix = matrix(In.vRight, In.vUp, In.vLook, In.vTranslation);
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

    float3 worldPos = vPosition.xyz;

    // 트렁크의 흔들림 계산
    float heightFactor = saturate(In.vPosition.y / 10.f); // 높이에 따른 가중치, 10.0은 트렁크의 대략적인 높이
    float swayAmount = sin(g_fTime * 1.5 + worldPos.x * 0.05 + worldPos.z * 0.05) * 0.02;
    swayAmount *= heightFactor * g_fWindStrength * 0.3f;

    vPosition.x += swayAmount * g_vWindDirection.x;
    vPosition.z += swayAmount * g_vWindDirection.z;

    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    matrix matPrevWV, matPrevWVP;
    matPrevWV = mul(g_WorldMatrix, g_PrevViewMatrix);
    matPrevWVP = mul(matPrevWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), TransformMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    Out.vLocalPos = float4(In.vPosition, 1.f);
    Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), TransformMatrix));
    Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);

    // 노말을 이용한 Velocity 계산
    float3 worldNormal = mul(Out.vNormal.xyz, (float3x3)g_WorldMatrix);
    float3 viewNormal = mul(worldNormal, (float3x3)g_ViewMatrix);
    float3 projNormal = mul(viewNormal, (float3x3)g_ProjMatrix);

    float3 currentViewPos = mul(vPosition, matWV).xyz;
    float3 prevViewPos = mul(vPosition, matPrevWV).xyz;

    float3 viewMotion = currentViewPos - prevViewPos;
    float motionAlongNormal = dot(viewMotion, viewNormal);

    float2 ndcMotion = projNormal.xy * motionAlongNormal;

    Out.vVelocity = ndcMotion * -0.3f;  // 스케일 조정
    return Out;
}
VS_OUT VS_LEAF(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;

    matrix TransformMatrix = matrix(In.vRight, In.vUp, In.vLook, In.vTranslation);
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

    float3 worldPos = vPosition.xyz;

    // 월드 위치에 따른 오프셋 생성
    float positionOffset = sin(worldPos.x * 0.1 + worldPos.z * 0.1) * 0.5 + 0.5;

    // 복합 움직임 생성
    float noise1 = sin(g_fTime * 1.5 + worldPos.x * 0.05 + worldPos.z * 0.05) * 0.05 * positionOffset;
    float noise2 = cos(g_fTime * 2.0 + worldPos.z * 0.1) * 0.03;
    float noise3 = sin(g_fTime * 2.5 + worldPos.x * 0.1) * 0.02;

    // 상하 움직임 추가
    float verticalNoise = sin(g_fTime + worldPos.x * 0.02 + worldPos.z * 0.02) * 0.01;

    float3 totalNoise = float3(
        noise1 * g_vWindDirection.x + noise3,
        verticalNoise + noise2 * 0.5,
        noise1 * g_vWindDirection.z + noise2
    ) * g_fWindStrength;

    // 버텍스 위치에 따른 가중치 (잎의 끝부분이 더 많이 움직이도록)
    float tipInfluence = saturate((worldPos.y - In.vPosition.y) * 0.5 + 0.5);
    vPosition.xyz += totalNoise * tipInfluence;

    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), TransformMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    Out.vLocalPos = float4(In.vPosition, 1.f);
    Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), TransformMatrix));
    Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);

    return Out;
}

struct VS_OUT_LIGHTDEPTH
{
    float4      vPosition : SV_POSITION;
    float2      vTexcoord : TEXCOORD0;
    float4      vProjPos : TEXCOORD1;
};

VS_OUT_LIGHTDEPTH VS_MAIN_LIGHTDEPTH(VS_IN In)
{
    VS_OUT_LIGHTDEPTH Out = (VS_OUT_LIGHTDEPTH)0;

    matrix TransformMatrix = matrix(In.vRight, In.vUp, In.vLook, In.vTranslation);
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vLocalPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
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
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vDiffuse.a < 0.1f)
        discard;

    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

    if (g_bDiffuse) Out.vDiffuse = vDiffuse;

    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 1.f);
    if (g_bSpecular) Out.vSpecular = vSpecular;

    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
    if (g_bEmissive) Out.vEmissive = vEmissive;
    if (g_bRoughness) Out.vRoughness = vRoughness;
    if (g_bMetalic) Out.vMetalic = vMetalic;

    return Out;
}

struct PS_OUT_COLOR
{
    vector vColor : SV_TARGET0;
};

PS_OUT_COLOR PS_COLOR(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR)0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vDiffuse.a < 0.1f)
        discard;

    if (g_bDiffuse) Out.vColor = vDiffuse;

    return Out;
}


PS_OUT PS_TILING(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float2 vTilingTexcoord = In.vTexcoord * 40.f;
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, vTilingTexcoord);
    if (vDiffuse.a < 0.1f)
        discard;

    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, vTilingTexcoord);

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, vTilingTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

    if (g_bDiffuse) Out.vDiffuse = vDiffuse;

    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 1.f);
    if (g_bSpecular) Out.vSpecular = vSpecular;

    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, vTilingTexcoord);
    vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, vTilingTexcoord);
    vector vMetalic = g_MetalicTexture.Sample(LinearSampler, vTilingTexcoord);
    if (g_bEmissive) Out.vEmissive = vEmissive;
    if (g_bRoughness) Out.vRoughness = vRoughness;
    if (g_bMetalic) Out.vMetalic = vMetalic;

    return Out;
}


PS_OUT PS_LEAF(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vDiffuse.a < 0.1f || vDiffuse.r < 0.9f)
        discard;

    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

    if (g_bDiffuse)
    {
        // 기본 잎 색상
        float3 baseColor = g_LeafCol;

        // UV 좌표를 이용한 그라데이션 효과
        float2 centeredUV = In.vTexcoord - 0.5;
        float distanceFromCenter = length(centeredUV);
        float gradient = 1.0 - smoothstep(0.0, 0.5, distanceFromCenter);

        // 밝은 색상 계산
        float3 brightColor = baseColor * 1.3; // 30% 더 밝게

        // 그라데이션에 따라 색상 혼합
        float3 finalColor = lerp(baseColor, brightColor, gradient);

        // 약간의 랜덤 변화 추가
        float randomness = frac(sin(dot(In.vPosition.xy, float2(12.9898, 78.233))) * 43758.5453);
        finalColor *= (0.9 + 0.2 * randomness); // ±10% 랜덤 변화

        Out.vDiffuse = float4(finalColor, 1.f);
    }

    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 1.f);
    if (g_bSpecular) Out.vSpecular = vSpecular;

    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
    if (g_bEmissive) Out.vEmissive = vEmissive;
    if (g_bRoughness) Out.vRoughness = vRoughness;
    if (g_bMetalic) Out.vMetalic = vMetalic;

    return Out;
}


PS_OUT_COLOR PS_LEAF_BLOOM(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR)0;

    vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vColor.a < 0.1f || vColor.r < 0.9f)
        discard;

    if (g_bDiffuse)
    {
        Out.vColor = float4(g_LeafCol * 0.5f, 1.f);
    }

    return Out;
}

PS_OUT_COLOR PS_TRUNK_BLOOM(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR)0;

    vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vColor.a < 0.1f /*|| vColor.r < 0.9f*/)
        discard;

    if (g_bDiffuse)
    {
        Out.vColor = vColor * 0.6f + 0.1f;
    }

    return Out;
}


PS_OUT PS_TRUNK(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vDiffuse.a < 0.1f)
        discard;

    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

    if (g_bDiffuse) Out.vDiffuse = (vDiffuse * 0.7f) + 0.1f;

    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 1.f);
    if (g_bSpecular) Out.vSpecular = vSpecular;

    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
    if (g_bEmissive) Out.vEmissive = vEmissive;
    if (g_bRoughness) Out.vRoughness = vRoughness;
    if (g_bMetalic) Out.vMetalic = vMetalic;

    return Out;
}

struct PS_IN_LIGHTDEPTH
{
    float4      vPosition : SV_POSITION;
    float2      vTexcoord : TEXCOORD0;
    float4      vProjPos : TEXCOORD1;
};

struct PS_OUT_LIGHTDEPTH
{
    vector      vLightDepth : SV_TARGET0;
};


PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN_LIGHTDEPTH In)
{
    PS_OUT_LIGHTDEPTH      Out = (PS_OUT_LIGHTDEPTH)0;

    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vDiffuse.a < 0.1f)
        discard;

    Out.vLightDepth = vector(In.vProjPos.w / 3000.f, 0.0f, 0.f, 0.f);

    return Out;
}

technique11 DefaultTechnique
{

    /* 특정 렌더링을 수행할 때 적용해야할 셰이더 기법의 셋트들의 차이가 있다. */
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Color_1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR();
    }

      pass Tmp_2 // 안씀
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR();
    }

      pass Tiling_3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_TILING();
    }

        pass LightDepth_4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_LIGHTDEPTH();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
    }

        pass Tree_5
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_LEAF();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_LEAF();
    }

         pass LeafBloom_6
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_LEAF();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_LEAF_BLOOM();
    }

    pass TrunkBloom_7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_TRUNK_BLOOM();
    }

        pass BloomTree_Trunk_8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_TRUNK();
    }

}