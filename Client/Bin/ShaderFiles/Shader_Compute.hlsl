//--------------------------------------------------------------------------------------
// File: BasicCompute11.hlsl
//
// This file contains the Compute Shader to perform array A + array B
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
RWStructuredBuffer<float4> gOutput : register(u0);

[numthreads(16, 16, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
    gOutput[0] = float4(1.0, 2.0, 3.0, 4.0);
}



technique11 ComputeResult
{
    pass P0
    {
        SetComputeShader(CompileShader(cs_5_0, CS()));
    }
}





//struct ComputeShaderData
//{
//    float3 data1;
//    float3 data2;
//};
//
//StructuredBuffer<ComputeShaderData> g_InputBuffer : register(t0);
//RWStructuredBuffer<ComputeShaderData> g_OutputBuffer : register(u0);
//
//[numthreads(1, 1, 1)]
//void CSMain(uint3 dispatchThreadId : SV_DispatchThreadID)
//{
//    // Input 데이터 읽기
//    ComputeShaderData input = g_InputBuffer[dispatchThreadId.x];
//
//    // 각 원소에 1을 더하기
//    input.data1 += float3(1.0f, 1.0f, 1.0f);
//    input.data2 += float3(1.0f, 1.0f, 1.0f);
//
//    // Output 데이터 쓰기
//    g_OutputBuffer[dispatchThreadId.x] = input;
//}


//#ifdef USE_STRUCTURED_BUFFERS
//
//struct BufType
//{
//    int i;
//    float f;
//#ifdef TEST_DOUBLE
//    double d;
//#endif    
//};
//
//StructuredBuffer<BufType> Buffer0 : register(t0);
//StructuredBuffer<BufType> Buffer1 : register(t1);
//RWStructuredBuffer<BufType> BufferOut : register(u0);
//
//[numthreads(1, 1, 1)]
//void CSMain(uint3 DTid : SV_DispatchThreadID)
//{
//    BufferOut[DTid.x].i = Buffer0[DTid.x].i + Buffer1[DTid.x].i;
//    BufferOut[DTid.x].f = Buffer0[DTid.x].f + Buffer1[DTid.x].f;
//#ifdef TEST_DOUBLE
//    BufferOut[DTid.x].d = Buffer0[DTid.x].d + Buffer1[DTid.x].d;
//#endif 
//}
//
//#else // The following code is for raw buffers
//
//ByteAddressBuffer Buffer0 : register(t0);
//ByteAddressBuffer Buffer1 : register(t1);
//RWByteAddressBuffer BufferOut : register(u0);
//
//[numthreads(1, 1, 1)]
//void CSMain(uint3 DTid : SV_DispatchThreadID)
//{
//#ifdef TEST_DOUBLE
//    int i0 = asint(Buffer0.Load(DTid.x * 16));
//    float f0 = asfloat(Buffer0.Load(DTid.x * 16 + 4));
//    double d0 = asdouble(Buffer0.Load(DTid.x * 16 + 8), Buffer0.Load(DTid.x * 16 + 12));
//    int i1 = asint(Buffer1.Load(DTid.x * 16));
//    float f1 = asfloat(Buffer1.Load(DTid.x * 16 + 4));
//    double d1 = asdouble(Buffer1.Load(DTid.x * 16 + 8), Buffer1.Load(DTid.x * 16 + 12));
//
//    BufferOut.Store(DTid.x * 16, asuint(i0 + i1));
//    BufferOut.Store(DTid.x * 16 + 4, asuint(f0 + f1));
//
//    uint dl, dh;
//    asuint(d0 + d1, dl, dh);
//
//    BufferOut.Store(DTid.x * 16 + 8, dl);
//    BufferOut.Store(DTid.x * 16 + 12, dh);
//#else
//    int i0 = asint(Buffer0.Load(DTid.x * 8));
//    float f0 = asfloat(Buffer0.Load(DTid.x * 8 + 4));
//    int i1 = asint(Buffer1.Load(DTid.x * 8));
//    float f1 = asfloat(Buffer1.Load(DTid.x * 8 + 4));
//
//    BufferOut.Store(DTid.x * 8, asuint(i0 + i1));
//    BufferOut.Store(DTid.x * 8 + 4, asuint(f0 + f1));
//#endif // TEST_DOUBLE
//}
//
//#endif // USE_STRUCTURED_BUFFERS