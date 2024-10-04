// ComputeShader.hlsl

// 입력 배열과 출력 배열을 정의합니다.





Buffer<float4> inputArrayA : register(t0);
Buffer<float4> inputArrayB : register(t1);
RWBuffer<float4> outputArray : register(u0);

// 각 컴퓨트 셰이더 스레드 그룹의 크기를 정의합니다.
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // 입력 배열의 인덱스를 결정합니다.

    // 입력 배열에서 값을 읽어와 더한 후 출력 배열에 저장합니다.
    outputArray[0] = inputArrayA[0] * inputArrayB[0];
}

