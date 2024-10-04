// ComputeShader.hlsl

// 입력 배열과 출력 배열을 정의합니다.
Buffer<float> inputArrayA : register(t0);
Buffer<float> inputArrayB : register(t1);
RWBuffer<float> outputArray : register(u0);

// 각 컴퓨트 셰이더 스레드 그룹의 크기를 정의합니다.
[numthreads(4, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // 입력 배열의 인덱스를 결정합니다.
    uint index = DTid.x;

    // 입력 배열에서 값을 읽어와 더한 후 출력 배열에 저장합니다.
    outputArray[index] = inputArrayA[index] * inputArrayB[index];
}

