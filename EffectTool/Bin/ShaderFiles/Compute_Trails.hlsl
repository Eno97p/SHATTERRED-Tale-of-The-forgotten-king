
cbuffer TrailConstants : register(b0)
{
    matrix ParentMat;    // 부모 매트릭스
    float3 vPivotPos;    // 피벗 위치
    uint m_iNumInstance; // 인스턴스 개수
};

// 구조화된 버퍼: 버텍스 매트릭스를 저장
StructuredBuffer<VTXMATRIX> Vertices : register(t0);
RWStructuredBuffer<VTXMATRIX> OutputVertices : register(u0);

// 구조화된 버퍼: 사이즈와 속도를 저장
StructuredBuffer<float3> Sizes : register(t1);
StructuredBuffer<float> Speeds : register(t2);

// VTXMATRIX 구조체 정의
struct VTXMATRIX
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    float2 vLifeTime;
    float vGravity;
};

// 컴퓨트 셰이더
[numthreads(16, 16, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint i = DTid.x;

    if (i >= m_iNumInstance)
        return;

    VTXMATRIX thisVertex = Vertices[i];

    if (i > 0)  // 첫 번째 인스턴스가 아닐 경우
    {
        VTXMATRIX frontVertex = Vertices[i - 1];

        // 매트릭스 로드
        matrix thisMat = matrix(thisVertex.vRight, thisVertex.vUp, thisVertex.vLook, thisVertex.vTranslation);
        matrix frontMat = matrix(frontVertex.vRight, frontVertex.vUp, frontVertex.vLook, frontVertex.vTranslation);

        // 구면 선형 보간 수행
        thisMat = XMMatrixSlerp(thisMat, frontMat, Speeds[i] * fDelta);

        // 벡터 정규화 및 크기 조정
        thisVertex.vTranslation = thisMat[3];
        thisVertex.vRight = normalize(thisMat[0]) * Sizes[i].x;
        thisVertex.vUp = normalize(thisMat[1]) * Sizes[i].y;
        thisVertex.vLook = normalize(thisMat[2]) * Sizes[i].z;

        // 수명 업데이트
        float ratio = float(i) / float(m_iNumInstance - 1);
        thisVertex.vLifeTime.y = thisVertex.vLifeTime.x * ratio;
    }
    else  // 첫 번째 인스턴스일 경우
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

    // 업데이트된 버텍스 저장
    OutputVertices[i] = thisVertex;
}