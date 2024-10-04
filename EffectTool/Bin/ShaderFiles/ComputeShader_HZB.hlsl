// ComputeShader_HZB.hlsl
Texture2D<float> gInput : register(t0);
RWTexture2D<float> gOutput : register(u0);

cbuffer cbSettings : register(b0)
{
    uint gInputWidth;
    uint gInputHeight;
    uint gMipLevel;
};

#define THREAD_GROUP_SIZE 16

groupshared float gCache[THREAD_GROUP_SIZE * THREAD_GROUP_SIZE];

[numthreads(THREAD_GROUP_SIZE, THREAD_GROUP_SIZE, 1)]
void CS_BuildHZB(uint3 groupId : SV_GroupID,
    uint3 groupThreadId : SV_GroupThreadID,
    uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint2 sourceSize = uint2(gInputWidth, gInputHeight);
    uint2 destSize = uint2(sourceSize.x / 2, sourceSize.y / 2);

    // 입력 텍스처에서 깊이 값 읽기
    if (dispatchThreadId.x < sourceSize.x && dispatchThreadId.y < sourceSize.y)
    {
        uint2 sourcePixel = dispatchThreadId.xy;
        float depth = gInput[sourcePixel];
        gCache[groupThreadId.y * THREAD_GROUP_SIZE + groupThreadId.x] = depth;
    }
    else
    {
        gCache[groupThreadId.y * THREAD_GROUP_SIZE + groupThreadId.x] = 0;
    }

    GroupMemoryBarrierWithGroupSync();

    // 2x2 블록의 최대 깊이 값 계산
    if (groupThreadId.x % 2 == 0 && groupThreadId.y % 2 == 0)
    {
        float depth0 = gCache[groupThreadId.y * THREAD_GROUP_SIZE + groupThreadId.x];
        float depth1 = gCache[groupThreadId.y * THREAD_GROUP_SIZE + groupThreadId.x + 1];
        float depth2 = gCache[(groupThreadId.y + 1) * THREAD_GROUP_SIZE + groupThreadId.x];
        float depth3 = gCache[(groupThreadId.y + 1) * THREAD_GROUP_SIZE + groupThreadId.x + 1];

        float maxDepth = max(max(depth0, depth1), max(depth2, depth3));

        uint2 writeCoord = dispatchThreadId.xy / 2;
        if (writeCoord.x < destSize.x && writeCoord.y < destSize.y)
        {
            gOutput[writeCoord] = maxDepth;
        }
    }
}

technique11 BuildHZB
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, CS_BuildHZB()));
    }
}