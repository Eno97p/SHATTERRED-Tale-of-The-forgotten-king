#include "Engine_Shader_Defines.hlsli"

cbuffer TimeBuffer : register(b0)
{
    float fTimeDelta;
};

struct VTXMATRIX
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    float2 vLifeTime;
    float vGravity;
};

StructuredBuffer<VTXMATRIX> g_Input : register(t0);
RWStructuredBuffer<VTXMATRIX> g_Output : register(u0);

[numthreads(256, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint i = DTid.x;

    VTXMATRIX vertex = g_Input[i];
    vertex.vLifeTime.y += fTimeDelta;

    // Update translation based on gravity
    vertex.vTranslation.y -= vertex.vGravity * fTimeDelta;

    float3 vDir = normalize(vertex.vTranslation.xyz - g_Input[i].vTranslation.xyz);

    float3 vRight = normalize(cross(vDir, float3(0.0f, 1.0f, 0.0f)));
    float3 vUp = normalize(cross(vDir, vRight));

    // Rotation logic (example)
    float angle = 360.0f * fTimeDelta;
    matrix RotationMatrix = matrix(
        float4(vRight,0.f),
        float4(vUp,0.f),
        float4(vDir,0.f),
        float4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    vertex.vRight = mul(RotationMatrix, vertex.vRight);
    vertex.vUp = mul(RotationMatrix, vertex.vUp);
    vertex.vLook = mul(RotationMatrix, vertex.vLook);

    g_Output[i] = vertex;
}