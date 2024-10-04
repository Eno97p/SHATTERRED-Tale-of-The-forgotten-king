// ComputeShader.hlsl

// �Է� �迭�� ��� �迭�� �����մϴ�.
Buffer<float> inputArrayA : register(t0);
Buffer<float> inputArrayB : register(t1);
RWBuffer<float> outputArray : register(u0);

// �� ��ǻƮ ���̴� ������ �׷��� ũ�⸦ �����մϴ�.
[numthreads(4, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // �Է� �迭�� �ε����� �����մϴ�.
    uint index = DTid.x;

    // �Է� �迭���� ���� �о�� ���� �� ��� �迭�� �����մϴ�.
    outputArray[index] = inputArrayA[index] * inputArrayB[index];
}

