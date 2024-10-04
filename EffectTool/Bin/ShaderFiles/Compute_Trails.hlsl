
cbuffer TrailConstants : register(b0)
{
    matrix ParentMat;    // �θ� ��Ʈ����
    float3 vPivotPos;    // �ǹ� ��ġ
    uint m_iNumInstance; // �ν��Ͻ� ����
};

// ����ȭ�� ����: ���ؽ� ��Ʈ������ ����
StructuredBuffer<VTXMATRIX> Vertices : register(t0);
RWStructuredBuffer<VTXMATRIX> OutputVertices : register(u0);

// ����ȭ�� ����: ������� �ӵ��� ����
StructuredBuffer<float3> Sizes : register(t1);
StructuredBuffer<float> Speeds : register(t2);

// VTXMATRIX ����ü ����
struct VTXMATRIX
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    float2 vLifeTime;
    float vGravity;
};

// ��ǻƮ ���̴�
[numthreads(16, 16, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint i = DTid.x;

    if (i >= m_iNumInstance)
        return;

    VTXMATRIX thisVertex = Vertices[i];

    if (i > 0)  // ù ��° �ν��Ͻ��� �ƴ� ���
    {
        VTXMATRIX frontVertex = Vertices[i - 1];

        // ��Ʈ���� �ε�
        matrix thisMat = matrix(thisVertex.vRight, thisVertex.vUp, thisVertex.vLook, thisVertex.vTranslation);
        matrix frontMat = matrix(frontVertex.vRight, frontVertex.vUp, frontVertex.vLook, frontVertex.vTranslation);

        // ���� ���� ���� ����
        thisMat = XMMatrixSlerp(thisMat, frontMat, Speeds[i] * fDelta);

        // ���� ����ȭ �� ũ�� ����
        thisVertex.vTranslation = thisMat[3];
        thisVertex.vRight = normalize(thisMat[0]) * Sizes[i].x;
        thisVertex.vUp = normalize(thisMat[1]) * Sizes[i].y;
        thisVertex.vLook = normalize(thisMat[2]) * Sizes[i].z;

        // ���� ������Ʈ
        float ratio = float(i) / float(m_iNumInstance - 1);
        thisVertex.vLifeTime.y = thisVertex.vLifeTime.x * ratio;
    }
    else  // ù ��° �ν��Ͻ��� ���
    {
        float4x4 parentMat = (float4x4)ParentMat;

        float3 vRight = normalize(parentMat[0].xyz);
        float3 vUp = normalize(parentMat[1].xyz);
        float3 vLook = normalize(parentMat[2].xyz);
        thisVertex.vLifeTime.y = 1.0f;

        float3 vPos = mul(float4(vPivotPos, 1.0f), ParentMat).xyz;

        thisVertex.vTranslation = float4(vPos, 1.0f);
        thisVertex.vRight = float4(vRight * Sizes[i].x, 0.0f);
        thisVertex.vUp = float4(vUp * Sizes[i].y, 0.0f);
        thisVertex.vLook = float4(vLook * Sizes[i].z, 0.0f);
    }

    // ������Ʈ�� ���ؽ� ����
    OutputVertices[i] = thisVertex;
}