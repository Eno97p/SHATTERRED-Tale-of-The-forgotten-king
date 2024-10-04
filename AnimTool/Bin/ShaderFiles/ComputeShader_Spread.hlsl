// ComputeShader.hlsl

struct VTXMATRIX 
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    float2 vLifeTime;
    float vGravity;
    float padding;
};

struct Instance_Value
{
    float deltaTime;
    float3 offsetPos;
    uint NumInstance;
    float3 OriginalPositions;
    float Speed;
    float OriginalSpeed;
    float OriginalGravity;
    float Size;
    float OriginalSize;
};

struct Output_Buffer
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    float2 vLifeTime;
    float vGravity;
    float Speed;
    float Size;
    int   isDead;
};


// 입력 배열과 출력 배열을 정의합니다.
StructuredBuffer<VTXMATRIX> inputArrayA : register(t0);
StructuredBuffer<Instance_Value> inputArrayB : register(t1);
RWStructuredBuffer<Output_Buffer> outputArray : register(u0);

// 각 컴퓨트 셰이더 스레드 그룹의 크기를 정의합니다.
[numthreads(128, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint i = DTid.x;
    if (i >= inputArrayA.Length) return;
    VTXMATRIX instance = inputArrayA[i];
    Instance_Value instanceValue = inputArrayB[0];

    instance.vLifeTime.y += instanceValue.deltaTime;
    float size = instanceValue.Size - instanceValue.deltaTime * 0.1f;
    if (size < 0.0f) size = 0.0f;

    instance.vGravity = instanceValue.OriginalGravity * (instance.vLifeTime.y / instance.vLifeTime.x);
    float3 dir = (instance.vTranslation.xyz - instanceValue.offsetPos) - float3(0.0f, instance.vGravity, 0.0f);

    float3 look = normalize(dir);
    float3 right = normalize(cross(look, float3(0.0f, 1.0f, 0.0f)));
    float3 up = normalize(cross(right, look));

    float angle = radians(360.0f) * instanceValue.deltaTime;
    float3x3 rotationMatrix = float3x3(
        cos(angle), 0, sin(angle),
        0, 1, 0,
        -sin(angle), 0, cos(angle)
    );

    right = mul(right, rotationMatrix);
    up = mul(up, rotationMatrix);
    look = mul(look, rotationMatrix);

    Output_Buffer output;
    output.vRight = float4(right * size, 0.0f);
    output.vUp = float4(up * size, 0.0f);
    output.vLook = float4(look * size, 0.0f);
    output.vTranslation = instance.vTranslation;
    output.vLifeTime = instance.vLifeTime;
    output.vGravity = instance.vGravity;
    output.Speed = instanceValue.Speed;
    output.Size = size;
    output.isDead = (instance.vLifeTime.y >= instance.vLifeTime.x) ? 1 : 0;
    outputArray[i] = output;

}

