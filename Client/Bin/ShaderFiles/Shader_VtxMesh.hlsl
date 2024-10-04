#include "Engine_Shader_Defines.hlsli"

/* 컨스턴트 테이블(상수테이블) */
matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix, g_PrevWorldMatrix, g_PrevViewMatrix;
texture2D   g_DiffuseTexture;
texture2D   g_NormalTexture;
texture2D   g_SpecularTexture;
texture2D   g_OpacityTexture;
texture2D   g_EmissiveTexture;
texture2D   g_RoughnessTexture;
texture2D   g_MetalicTexture;
texture2D   g_DisolveTexture;
texture2D   g_BlurTexture;
float4 g_DisolveColor = float4(0.f, 1.f, 1.f, 1.f);

float4 g_vCamPosition;

float g_DisolveValue = 1.f;
bool g_bDiffuse = false;
bool g_bNormal = false;
bool g_bSpecular = false;
bool g_bOpacity = false;
bool g_bEmissive = false;
bool g_bRoughness = false;
bool g_bMetalic = false;
bool g_MotionBlur = false;

float g_TexcoordY = 1.f;

float4		g_fColor = { 1.f, 1.f, 1.f, 1.f };

struct VS_IN
{
    float3      vPosition : POSITION;
    float3      vNormal : NORMAL;
    float2      vTexcoord : TEXCOORD0;
    float3      vTangent : TANGENT;
};

struct VS_OUT
{
    float4      vPosition : SV_POSITION;
    float4      vNormal : NORMAL;
    float2      vTexcoord : TEXCOORD0;
    float4      vProjPos : TEXCOORD1;
    float4      vLocalPos : TEXCOORD2;
    float4      vTangent : TANGENT;
    float4      vBinormal : BINORMAL;
    float2		vVelocity : TEXCOORD3;
};

/* 정점 셰이더 :  /*
/* 1. 정점의 위치 변환(월드, 뷰, 투영).*/
/* 2. 정점의 구성정보를 변경한다. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT      Out = (VS_OUT)0;

    matrix      matWV, matWVP, matPrevWV, matPrevWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    matPrevWV = mul(g_PrevWorldMatrix, g_PrevViewMatrix);
    matPrevWVP = mul(matPrevWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    Out.vLocalPos = float4(In.vPosition, 1.f);
    Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);

    vector vPrevPos = mul(float4(In.vPosition, 1.f), matPrevWVP);
    Out.vVelocity = (Out.vPosition.xy / Out.vPosition.w) - (vPrevPos.xy / vPrevPos.w);
    Out.vVelocity *= -2.f;
    return Out;
}

struct VS_OUT_LIGHTDEPTH
{
    float4		vPosition : SV_POSITION;
    float2		vTexcoord : TEXCOORD0;
    float4		vProjPos : TEXCOORD1;
};

VS_OUT_LIGHTDEPTH VS_MAIN_LIGHTDEPTH(VS_IN In)
{
    VS_OUT_LIGHTDEPTH		Out = (VS_OUT_LIGHTDEPTH)0;

    matrix		matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}

/* TriangleList인 경우 : 정점 세개를 받아서 w나누기를 각 정점에 대해서 수행한다. */
/* 뷰포트(윈도우좌표로) 변환. */
/* 래스터라이즈 : 정점으로 둘러쌓여진 픽셀의 정보를, 정점을 선형보간하여 만든다. -> 픽셀이 만들어졌다!!!!!!!!!!!! */


struct PS_IN
{
    float4      vPosition : SV_POSITION;
    float4      vNormal : NORMAL;
    float2      vTexcoord : TEXCOORD0;
    float4      vProjPos : TEXCOORD1;
    float4      vLocalPos : TEXCOORD2;
    float4      vTangent : TANGENT;
    float4      vBinormal : BINORMAL;
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
    PS_OUT Out = (PS_OUT)0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);

    if (g_bOpacity) vDiffuse.a *= vOpacity.r;
    if (vDiffuse.a < 0.1f)
        discard;

    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

    if (g_bDiffuse) Out.vDiffuse = vDiffuse;

    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);
    if (g_bSpecular) Out.vSpecular = vSpecular;

    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
    if (g_bEmissive) Out.vEmissive = vEmissive;
    if (g_bRoughness) Out.vRoughness = vRoughness;
    if (g_bMetalic) Out.vMetalic = vMetalic;

    if (g_MotionBlur)
    {
        Out.vVelocity = In.vVelocity;
    }

    return Out;
}

//PS_OUT PS_WHISPERSWORD(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT)0;
//
//    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
//    vector vOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);
//
//    if (g_bOpacity) vDiffuse.a *= vOpacity.r;
//    if (vDiffuse.a < 0.1f)
//        discard;
//
//    if (vDiffuse.r + vDiffuse.g + vDiffuse.b < 0.4f) discard;
//
//    if (g_bDiffuse) Out.vDiffuse = vDiffuse;
//    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);
//
//    Out.vDiffuse.a = (Out.vDiffuse.r + Out.vDiffuse.g + Out.vDiffuse.b) / 3.f;
//    if (Out.vDiffuse.a < 0.3f) discard;
//
//    if (g_MotionBlur)
//    {
//        Out.vVelocity = In.vVelocity;
//    }
//
//    return Out;
//}

PS_OUT PS_WEAPON(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);

    if (g_bOpacity) vDiffuse.a *= vOpacity.r;
    if (vDiffuse.a < 0.1f)
        discard;

    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

    if (g_bDiffuse) Out.vDiffuse = vDiffuse;

    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);
    if (g_bSpecular) Out.vSpecular = vSpecular;

    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
    if (g_bEmissive) Out.vEmissive = vEmissive;
    if (g_bRoughness) Out.vRoughness = vRoughness;
    else Out.vRoughness = vector(0.7f, 0.7f, 0.7f, 1.f);
    if (g_bMetalic) Out.vMetalic = vMetalic;
    else Out.vMetalic = vector(1.f, 1.f, 1.f, 1.f);

    vector vDisolve = g_DisolveTexture.Sample(LinearSampler, In.vTexcoord);
    float disolveValue = (vDisolve.r + vDisolve.g + vDisolve.b) / 3.f;
    if (g_MotionBlur)
    {
        Out.vVelocity = In.vVelocity;
    }
    if ((g_DisolveValue - disolveValue) > 0.1f)
    {
        return Out;
    }
    else if (disolveValue > g_DisolveValue)
    {
        discard;
    }
    else
    {
        Out.vDiffuse = g_DisolveColor;
    }

    return Out;
}

struct PS_OUT_COLOR
{
    vector vColor : SV_TARGET0;
};

PS_OUT_COLOR PS_BLOOM(PS_IN In)
{
	PS_OUT_COLOR Out = (PS_OUT_COLOR)0;
	Out.vColor = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
	return Out;
}

PS_OUT_COLOR PS_BLUR(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR)0;
    Out.vColor = g_BlurTexture.Sample(LinearSampler, In.vTexcoord);
    return Out;
}

PS_OUT_COLOR PS_WEAPON_REFLECTION(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR)0;

    vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vColor.a < 0.1f)
        discard;

    if (g_bDiffuse) Out.vColor = vColor;

    vector vDisolve = g_DisolveTexture.Sample(LinearSampler, In.vTexcoord);
    float disolveValue = (vDisolve.r + vDisolve.g + vDisolve.b) / 3.f;
    if ((g_DisolveValue - disolveValue) > 0.1f)
    {
        return Out;
    }
    else if (disolveValue > g_DisolveValue)
    {
        discard;
    }
    else
    {
        Out.vColor = g_DisolveColor;
    }

    return Out;
}

PS_OUT_COLOR PS_TARGETLOCK(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR)0;
    Out.vColor = g_fColor;
    return Out;

}

PS_OUT_COLOR PS_ASPIRATION(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR)0;

    vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vColor.a = (vColor.r + vColor.g + vColor.b) / 3.f;
    if (vColor.a < 0.1f || In.vTexcoord.y > g_TexcoordY || In.vTexcoord.y < g_TexcoordY - 0.3f)
        discard;

    if (g_bDiffuse) Out.vColor = float4(0.5f, 0.5f, 1.f, 1.f);

    return Out;
}

PS_OUT PS_SPHERE(PS_IN In)
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
    Out.vDiffuse = float4(0.5f, 0.5f, 1.f, 1.f);

    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);
    if (g_bSpecular) Out.vSpecular = vSpecular;

    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
    if (g_bEmissive) Out.vEmissive = vEmissive;
    if (g_bRoughness) Out.vRoughness = vRoughness;
    if (g_bMetalic) Out.vMetalic = vMetalic;
    if (g_MotionBlur)
    {
        Out.vVelocity = In.vVelocity;
    }

    return Out;
}

PS_OUT_COLOR PS_DECAL(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR)0;
    Out.vColor = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    return Out;
}

PS_OUT_COLOR PS_ITEM(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR)0;

    // 뷰 공간으로 변환
    float3 positionVS = mul(float4(In.vPosition.xyz, 1.0), g_ViewMatrix).xyz;
    float3 normalVS = normalize(mul(In.vNormal.xyz, (float3x3)g_ViewMatrix));

    // 뷰 공간에서의 뷰 방향 (항상 (0, 0, -1))
    float3 viewDirVS = float3(0, 0, -1);

    // 외곽선 강도 계산
    float NdotV = dot(normalVS, viewDirVS);
    float edgeStrength = 1.0 - abs(NdotV);

    // 깊이 기반 외곽선 강화
    float depthEdge = 1.0 - saturate(positionVS.z * 0.1); // 0.1은 조절 가능한 값입니다.

    // 최종 외곽선 강도
    float finalEdgeStrength = max(edgeStrength, depthEdge);

    // 외곽선 임계값 설정
    float edgeThreshold = 0.6; // 이 값을 조정하여 외곽선의 두께를 변경할 수 있습니다.

    // 부드러운 외곽선 전환을 위한 smoothstep 사용
    float smoothEdge = smoothstep(edgeThreshold - 0.01, edgeThreshold + 0.01, finalEdgeStrength);

    // 카메라 방향에 따른 처리
    float facingFactor = saturate(NdotV * 0.5 + 0.5);
    smoothEdge *= facingFactor;

    // 외곽선 색상 설정 (흰색)
    float4 edgeColor = float4(1, 1, 1, smoothEdge);

    // 최종 색상 출력
    Out.vColor = edgeStrength > edgeThreshold ? edgeColor : float4(0, 0, 0, 0);

    return Out;
}

PS_OUT PS_DISOLVE(PS_IN In)
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
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.0f, 1.f);
    if (g_bSpecular) Out.vSpecular = vSpecular;

    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    vector vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMetalic = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
    if (g_bEmissive) Out.vEmissive = vEmissive;
    if (g_bRoughness) Out.vRoughness = vRoughness;
    if (g_bMetalic) Out.vMetalic = vMetalic;

    vector vDisolve = g_DisolveTexture.Sample(LinearSampler, In.vTexcoord);
    float disolveValue = (vDisolve.r + vDisolve.g + vDisolve.b) / 3.f;
    if (g_MotionBlur)
    {
        Out.vVelocity = In.vVelocity;
    }
    if ((g_DisolveValue - disolveValue) > 0.1f)
    {
        return Out;
    }
    else if (disolveValue > g_DisolveValue)
    {
        discard;
    }
    else
    {
        Out.vDiffuse = g_DisolveColor;
    }

    return Out;
}

PS_OUT_COLOR PS_SAVEPOINT(PS_IN In)
{
    PS_OUT_COLOR Out = (PS_OUT_COLOR)0;


    Out.vColor = float4(0.f, 1.f, 0.f, 1.f);

    return Out;
}

struct PS_IN_LIGHTDEPTH
{
    float4		vPosition : SV_POSITION;
    float2		vTexcoord : TEXCOORD0;
    float4		vProjPos : TEXCOORD1;
};

struct PS_OUT_LIGHTDEPTH
{
    vector		vLightDepth : SV_TARGET0;
};

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN_LIGHTDEPTH In)
{
    PS_OUT_LIGHTDEPTH		Out = (PS_OUT_LIGHTDEPTH)0;

    vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

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

    //pass WhisperSword_1
    //{
    //   SetRasterizerState(RS_Default);
    //   SetDepthStencilState(DSS_Default, 0);
    //   SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

    //   /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
    //   VertexShader = compile vs_5_0 VS_MAIN();
    //   GeometryShader = NULL;
    //   HullShader = NULL;
    //   DomainShader = NULL;
    //   PixelShader = compile ps_5_0 PS_WHISPERSWORD();
    //}
    pass LightDepth_1
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

    pass Weapons_2
    {
       SetRasterizerState(RS_NoCull);
       SetDepthStencilState(DSS_Default, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_WEAPON();
    }

    pass Bloom_3
    {
       SetRasterizerState(RS_NoCull);
       SetDepthStencilState(DSS_Default, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_BLOOM();
    }

    pass Blur_4
    {
       SetRasterizerState(RS_NoCull);
       SetDepthStencilState(DSS_Default, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_BLUR();
    }
        pass Weapons_Reflection_5
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_None_Test_None_Write, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WEAPON_REFLECTION();
    }

    pass TargetLockPass_6
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_None_Test_None_Write, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_TARGETLOCK();
    }

    pass Aspiration_7
    {
       SetRasterizerState(RS_NoCull);
       SetDepthStencilState(DSS_Default, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_ASPIRATION();
    }

    pass Decal_8
    {
       SetRasterizerState(RS_NoCull);
       SetDepthStencilState(DSS_Default, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_DECAL();
    }

    pass Sphere_9
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_Default, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_SPHERE();
    }

    pass Item_10
    {
       SetRasterizerState(RS_Default);
       SetDepthStencilState(DSS_Default, 0);
       SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_ITEM();
    }

    pass Disolve_11
    {
       SetRasterizerState(RS_NoCull);
       SetDepthStencilState(DSS_Default, 0);
       SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

       /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
       VertexShader = compile vs_5_0 VS_MAIN();
       GeometryShader = NULL;
       HullShader = NULL;
       DomainShader = NULL;
       PixelShader = compile ps_5_0 PS_DISOLVE();
    }

     pass SavePoint_12
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        /* 어떤 셰이덜르 국동할지. 셰이더를 몇 버젼으로 컴파일할지. 진입점함수가 무엇이찌. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SAVEPOINT();
    }


}

