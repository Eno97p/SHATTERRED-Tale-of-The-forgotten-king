const int perm[256] =
{
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95,
    96, 53, 194, 233, 7, 225, 140, 36, 103, 30,
    69, 142, 8, 99, 37, 240, 21, 10, 23, 190,
    6, 148, 247, 120, 234, 75, 0, 26, 197, 62,
    94, 252, 219, 203, 117, 35, 11, 32, 57, 177,
    33, 88, 237, 149, 56, 87, 174, 20, 125, 136,
    171, 168, 68, 175, 74, 165, 71, 134, 139, 48,
    27, 166, 77, 146, 158, 231, 83, 111, 229, 122,
    60, 211, 133, 230, 220, 105, 92, 41, 55, 46,
    245, 40, 244, 102, 143, 54, 65, 25, 63, 161,
    1, 216, 80, 73, 209, 76, 132, 187, 208, 89,
    18, 169, 200, 196, 135, 130, 116, 188, 159, 86,
    164, 100, 109, 198, 173, 186, 3, 64, 52, 217,
    226, 250, 124, 123, 5, 202, 38, 147, 118, 126,
    255, 82, 85, 212, 207, 206, 59, 227, 47, 16,
    58, 17, 182, 189, 28, 42, 223, 183, 170, 213,
    119, 248, 152, 2, 44, 154, 163, 70, 221, 153,
    101, 155, 167, 43, 172, 9, 129, 22, 39, 253,
    19, 98, 108, 110, 79, 113, 224, 232, 178, 185,
    112, 104, 218, 246, 97, 228, 251, 34, 242, 193,
    238, 210, 144, 12, 191, 179, 162, 241, 81, 51,
    145, 235, 249, 14, 239, 107, 49, 192, 214, 31,
    181, 199, 106, 157, 184, 84, 204, 176, 115, 121,
    50, 45, 127, 4, 150, 254, 138, 236, 205, 93,
    222, 114, 67, 29, 24, 72, 243, 141, 128, 195,
    78, 66, 215, 61, 156, 180
};

int hash(int x, int y)
{
    return perm[(perm[x % 255] + y) % 255];
}

float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

float fade(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float grad(int hash, float x, float y)
{
    int h = hash & 7; // Convert low 3 bits of hash code
    float u = h < 4 ? x : y; // into 8 simple gradient directions,
    float v = h < 4 ? y : x; // and compute the dot product with (x,y).
    return ((h & 1) != 0 ? -u : u) + ((h & 2) != 0 ? -2.0f * v : 2.0f * v);
}

float perlinNoise(float2 pos)
{
    // Calculate the integer coordinates
    int X = (int) floor(pos.x) & 255;
    int Y = (int) floor(pos.y) & 255;
    
    // Calculate the fractional part of pos
    float fx = pos.x - floor(pos.x);
    float fy = pos.y - floor(pos.y);

    // Calculate fade curves for fx and fy
    float u = fade(fx);
    float v = fade(fy);

    // Hash coordinates of the 4 square corners
    int aa = hash(X, Y);
    int ab = hash(X, Y + 1);
    int ba = hash(X + 1, Y);
    int bb = hash(X + 1, Y + 1);

    // And add blended results from the 4 corners of the square
    float res = lerp(
        lerp(grad(aa, fx, fy), grad(ba, fx - 1, fy), u),
        lerp(grad(ab, fx, fy - 1), grad(bb, fx - 1, fy - 1), u),
        v
    );

    return res;
}

float4x4 QuaternionToMatrix(float4 q)
{
    float4x4 m;
    float x2 = q.x * q.x;
    float y2 = q.y * q.y;
    float z2 = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    m[0][0] = 1 - 2 * (y2 + z2);
    m[0][1] = 2 * (xy - wz);
    m[0][2] = 2 * (xz + wy);
    m[0][3] = 0;

    m[1][0] = 2 * (xy + wz);
    m[1][1] = 1 - 2 * (x2 + z2);
    m[1][2] = 2 * (yz - wx);
    m[1][3] = 0;

    m[2][0] = 2 * (xz - wy);
    m[2][1] = 2 * (yz + wx);
    m[2][2] = 1 - 2 * (x2 + y2);
    m[2][3] = 0;

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;

    return m;
}

// 행렬에서 쿼터니언 추출하는 함수
float4 MatrixToQuaternion(float4x4 m)
{
    float4 q;
    float tr = m[0][0] + m[1][1] + m[2][2];

    if (tr > 0)
    {
        float S = sqrt(tr + 1.0) * 2;
        q.w = 0.25 * S;
        q.x = (m[2][1] - m[1][2]) / S;
        q.y = (m[0][2] - m[2][0]) / S;
        q.z = (m[1][0] - m[0][1]) / S;
    }
    else if ((m[0][0] > m[1][1]) && (m[0][0] > m[2][2]))
    {
        float S = sqrt(1.0 + m[0][0] - m[1][1] - m[2][2]) * 2;
        q.w = (m[2][1] - m[1][2]) / S;
        q.x = 0.25 * S;
        q.y = (m[0][1] + m[1][0]) / S;
        q.z = (m[0][2] + m[2][0]) / S;
    }
    else if (m[1][1] > m[2][2])
    {
        float S = sqrt(1.0 + m[1][1] - m[0][0] - m[2][2]) * 2;
        q.w = (m[0][2] - m[2][0]) / S;
        q.x = (m[0][1] + m[1][0]) / S;
        q.y = 0.25 * S;
        q.z = (m[1][2] + m[2][1]) / S;
    }
    else
    {
        float S = sqrt(1.0 + m[2][2] - m[0][0] - m[1][1]) * 2;
        q.w = (m[1][0] - m[0][1]) / S;
        q.x = (m[0][2] + m[2][0]) / S;
        q.y = (m[1][2] + m[2][1]) / S;
        q.z = 0.25 * S;
    }

    return q;
}

// 쿼터니언 SLERP 함수
float4 QuaternionSlerp(float4 q1, float4 q2, float t)
{
    // q1과 q2를 정규화
    q1 = normalize(q1);
    q2 = normalize(q2);

    // 내적 계산
    float dotProduct = dot(q1, q2);

    // 두 쿼터니언이 너무 가까우면 선형 보간
    if (dotProduct > 0.9995f)
    {
        return normalize(lerp(q1, q2, t));
    }

    // 각도 계산
    float angle = acos(dotProduct);
    
    // SLERP
    return (sin((1 - t) * angle) * q1 + sin(t * angle) * q2) / sin(angle);
}

// 메인 SLERP 함수
float4x4 MatrixSlerp(float4x4 matStart, float4x4 matEnd, float t)
{
    // 행렬에서 회전 부분을 쿼터니언으로 추출
    float4 qStart = MatrixToQuaternion(matStart);
    float4 qEnd = MatrixToQuaternion(matEnd);

    // 쿼터니언 구면 선형 보간
    float4 qInterpolated = QuaternionSlerp(qStart, qEnd, t);

    // 행렬에서 Right 벡터, Up 벡터, Look 벡터 추출
    float3 vStartRight = matStart[0].xyz;
    float3 vEndRight = matEnd[0].xyz;
    float3 vStartUp = matStart[1].xyz;
    float3 vEndUp = matEnd[1].xyz;
    float3 vStartLook = matStart[2].xyz;
    float3 vEndLook = matEnd[2].xyz;
    float3 vStartTranslation = matStart[3].xyz;
    float3 vEndTranslation = matEnd[3].xyz;

    // Right 벡터, Up 벡터, Look, Position 벡터 선형 보간
    float3 vInterpolatedRight = lerp(vStartRight, vEndRight, t);
    float3 vInterpolatedUp = lerp(vStartUp, vEndUp, t);
    float3 vInterpolatedLook = lerp(vStartLook, vEndLook, t);
    float3 vInterpolatedTranslation = lerp(vStartTranslation, vEndTranslation, t);

    // 보간된 회전, 이동, Right 벡터, Up 벡터, Look 벡터를 조합하여 최종 행렬 생성
    float4x4 matResult = QuaternionToMatrix(qInterpolated);
    matResult[0].xyz = normalize(vInterpolatedRight);
    matResult[1].xyz = normalize(vInterpolatedUp);
    matResult[2].xyz = normalize(vInterpolatedLook);
    matResult[3].xyz = vInterpolatedTranslation;

    return matResult;
}
