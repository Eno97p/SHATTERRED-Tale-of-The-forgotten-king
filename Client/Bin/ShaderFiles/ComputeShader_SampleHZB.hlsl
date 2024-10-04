// ComputeShader_HZB.hlsl
Texture2D<float> gHZBTexture : register(t0);
RWStructuredBuffer<float> gOutputBuffer : register(u0);

cbuffer cbSettings
{
    uint gScreenWidth;
    uint gScreenHeight;
    uint gMipLevel;
};

SamplerState gSamLinearClamp : register(s0);

[numthreads(16, 16, 1)]
void CS_SampleHZB(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    if (dispatchThreadId.x >= gScreenWidth || dispatchThreadId.y >= gScreenHeight)
        return;

    float2 uv = float2(dispatchThreadId.xy) / float2(gScreenWidth, gScreenHeight);
    float depth = gHZBTexture.SampleLevel(gSamLinearClamp, uv, gMipLevel);

    uint index = dispatchThreadId.y * gScreenWidth + dispatchThreadId.x;
    gOutputBuffer[index] = depth;
}

technique11 BuildHZB
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, CS_SampleHZB()));
    }
}