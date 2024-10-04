
sampler PointSampler = sampler_state
{
	filter = min_mag_mip_Point;
	AddressU = WRAP;
	AddressV = WRAP;
};


sampler LinearSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = WRAP;
	AddressV = WRAP;
};

//�Ʒ�, ��ƼŬ ���.
sampler LinearWrap = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

sampler LinearClamp = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
};

//Cloud
sampler CloudSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = WRAP;
    AddressV = WRAP;
    AddressW = WRAP;
};


RasterizerState		RS_Default
{
	FillMode = SOLID;
	CullMode = BACK;
	FrontCounterClockwise = false;
};

RasterizerState		RS_Cull_CW
{
	FillMode = SOLID;
	CullMode = FRONT;
	FrontCounterClockwise = false;
};

RasterizerState RS_NoCull
{
    FillMode = SOLID;
    CullMode = NONE;
};

RasterizerState		RS_Wireframe
{
	FillMode = Wireframe;
	CullMode = BACK;
	FrontCounterClockwise = false;
};

DepthStencilState	DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = LESS_EQUAL;
};

DepthStencilState DSS_None_Test_None_Write
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

DepthStencilState DSS_Less_Equal_Test_Write
{
    DepthEnable = true;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
    StencilEnable = false;
};

//vector		vSrcColor = vector(1.f, 0.f, 0.f, 0.5f);
//vector		vDestColor = vector(0.f, 1.f, 0.f, 1.f);

//vector		vResult = vSrcColor.rgb * vSrcColor.a + 
//	vDestColor * (1.f - vSrcColor.a);

BlendState		BS_Default
{
	BlendEnable[0] = false;
};

BlendState		BS_AlphaBlend
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

BlendState CloudBlendState
{
    BlendEnable[0] = TRUE;
    SrcBlend[0] = SRC_ALPHA;
    DestBlend[0] = INV_SRC_ALPHA;
    BlendOp[0] = ADD;
    SrcBlendAlpha[0] = ONE;
    DestBlendAlpha[0] = INV_SRC_ALPHA;
    BlendOpAlpha[0] = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

BlendState BS_ParticleBlend
{
    BlendEnable[0] = true;
    SrcBlend[0] = Src_Alpha;
    DestBlend[0] = ONE;
    BlendOp[0] = Add;
    SrcBlendAlpha[0] = ONE;
    DestBlendAlpha[0] = ZERO;
    BlendOpAlpha[0] = Add;
    RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState DS_Particle
{
    DepthEnable = true;
    DepthWriteMask = ZERO;
    DepthFunc = LESS;
};

BlendState		BS_Blend
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;	
	SrcBlend = One;
	DestBlend = One;
	BlendOp = Add;
};


float4 CatmullRom(float4 P0, float4 P1, float4 P2, float4 P3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

    float4 result = (2.0f * P1) +
                    (-P0 + P2) * t +
                    (2.0f * P0 - 5.0f * P1 + 4.0f * P2 - P3) * t2 +
                    (-P0 + 3.0f * P1 - 3.0f * P2 + P3) * t3;

    return result * 0.5f;
}

float2 PerlinRandom(float2 st)
{
    st = float2(dot(st, float2(127.1, 311.7)),
                dot(st, float2(269.5, 183.3)));
    return -1.0 + 2.0 * frac(sin(st) * 43758.5453123);
}

float PerlinNoise(float2 st)
{
    float2 i = floor(st);
    float2 f = frac(st);
    float2 u = f * f * (3.0 - 2.0 * f);
    return lerp(
        lerp(dot(PerlinRandom(i + float2(0.0, 0.0)), f - float2(0.0, 0.0)),
             dot(PerlinRandom(i + float2(1.0, 0.0)), f - float2(1.0, 0.0)), u.x),
        lerp(dot(PerlinRandom(i + float2(0.0, 1.0)), f - float2(0.0, 1.0)),
             dot(PerlinRandom(i + float2(1.0, 1.0)), f - float2(1.0, 1.0)), u.x),
        u.y);
}

float2 RadialShear(float2 texCoord, float2 center, float strength, float offset) //���׶��� �ְ������
{
    float2 toCenter = texCoord - center;
    float distance = length(toCenter);
    
    // �Ÿ��� 0�� ��� (�߽����� �ִ� ���) NaN�� �����ϱ� ���� ó��
    if (distance > 0.0001f)
    {
        float2 normalizedDir = toCenter / distance;
        float shearAmount = distance * strength + offset;
        float2 shearOffset = normalizedDir * shearAmount;
        return texCoord + shearOffset;
    }
    else
    {
        return texCoord;
    }
}

float3 CalculateFresnel(float3 normal, float3 viewDir, float fresnelPower, float3 baseColor, float3 fresnelColor)
{
    // ��ְ� �� ���� ���͸� ����ȭ�մϴ�.
    normal = normalize(normal);
    viewDir = normalize(viewDir);

    // Fresnel ����� ����մϴ�.
    float fresnelFactor = pow(1.0 - dot(viewDir, normal), fresnelPower);

    // Fresnel ����� ����Ͽ� ������ ȥ���մϴ�.
    float3 finalColor = lerp(baseColor, fresnelColor, fresnelFactor);

    return finalColor;
}