// ComputeShader.hlsl

// �Է� �迭�� ��� �迭�� �����մϴ�.





Buffer<float4> inputArrayA : register(t0);
Buffer<float4> inputArrayB : register(t1);
RWBuffer<float4> outputArray : register(u0);

// �� ��ǻƮ ���̴� ������ �׷��� ũ�⸦ �����մϴ�.
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // �Է� �迭�� �ε����� �����մϴ�.

    // �Է� �迭���� ���� �о�� ���� �� ��� �迭�� �����մϴ�.
    outputArray[0] = inputArrayA[0] * inputArrayB[0];
}

