#include "..\Public\Cascade.h"
#include "GameInstance.h"

CCascade::CCascade()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCascade::Initialize()
{
	m_vPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vPoints[4] = _float3(-1.f, 1.f, 0.001f);
	m_vPoints[5] = _float3(1.f, 1.f, 0.001f);
	m_vPoints[6] = _float3(1.f, -1.f, 0.001f);
	m_vPoints[7] = _float3(-1.f, -1.f, 0.001f);

	m_vPoints[8] = _float3(-1.f, 1.f, 0.5f);
	m_vPoints[9] = _float3(1.f, 1.f, 0.5f);
	m_vPoints[10] = _float3(1.f, -1.f, 0.5f);
	m_vPoints[11] = _float3(-1.f, -1.f, 0.5f);

	m_vPoints[12] = _float3(-1.f, 1.f, 1.f);
	m_vPoints[13] = _float3(1.f, 1.f, 1.f);
	m_vPoints[14] = _float3(1.f, -1.f, 1.f);
	m_vPoints[15] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CCascade::Update()
{
	_matrix ViewMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);
	_matrix ProjMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ);

	_float4	vPoints[16] = {};

	for (size_t i = 0; i < 16; i++)
	{
		XMStoreFloat4(&vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), ProjMatrixInverse));		
	}
	for (size_t i = 0; i < 16; i++)
		XMStoreFloat4(&m_vWorldPoints[i], XMVector3Transform(XMLoadFloat4(&vPoints[i]), ViewMatrixInverse));

	Make_Planes(m_vWorldPoints, m_vWorldPlanes);

	m_fTotalTime += m_pGameInstance->Get_TimeDelta(TEXT("Timer_Default"));
}

void CCascade::Transform_ToLocalSpace(_fmatrix WorldMatrixInv)
{
	_float4		vLocalPoints[24] = {};

	for (size_t i = 0; i < 24; i++)
		XMStoreFloat4(&vLocalPoints[i], XMVector3Transform(XMLoadFloat4(&m_vWorldPoints[i]), WorldMatrixInv));

	Make_Planes(vLocalPoints, m_vLocalPlanes);
}


_bool CCascade::isIn_WorldCascade(_fvector vPosition, _float fRange)
{
	for (size_t i = 0; i < 6; i++)
	{
		_vector	vResult = XMPlaneDotCoord(XMLoadFloat4(&m_vWorldPlanes[i]), vPosition);
		_float	fResult = XMVectorGetX(vResult);
		if (fRange < fResult)
			return false;
	}
	return true;
}


_bool CCascade::isIn_LocalCascade(_fvector vPosition, _float fRange)
{
	for (size_t i = 0; i < 6; i++)
	{		
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vLocalPlanes[i]), vPosition)))
			return false;
	}
	return true;
}

_uint CCascade::Get_CascadeNum(_fvector vPosition, _float fRange)
{
	for (size_t i = 0; i < 18; i++)
	{
		_vector	vResult = XMPlaneDotCoord(XMLoadFloat4(&m_vWorldPlanes[i]), vPosition);
		_float	fResult = XMVectorGetX(vResult);
		if (0.f >= fResult)
		{
			if (i < 6)
			{
				return FRUSTUM_NEAR;
			}
			else if (i < 12)
			{
				return FRUSTUM_MIDDLE;
			}
			else
			{
				return FRUSTUM_FAR;
			}
		}			
	}
	return FRUSTUM_END;
}

bool CCascade::isVisible(_vector vPos, PxActor* actor)
{
    // 거리 기반 컬링
    _vector vCamPos = m_pGameInstance->Get_CamPosition();
    float distanceToCamera = XMVector3Length(XMVectorSubtract(vPos, vCamPos)).m128_f32[0];
    if (distanceToCamera > m_maxVisibleDistance)
        return false;

    // 바운딩 박스 정보 얻기
    PxBounds3 bounds = actor->getWorldBounds();
    PxVec3 center = bounds.getCenter();
    PxVec3 extents = bounds.getExtents();

    // 바운딩 구를 사용한 프러스텀 컬링
    float radius = extents.magnitude();
    if (!isIn_WorldCascade(XMLoadFloat3(&XMFLOAT3(center.x, center.y, center.z)), radius))
        return false;

    // 뷰 프로젝션 변환
    _matrix viewProjMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW) *
        m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);

    // 바운딩 박스의 8개 코너를 변환
    std::vector<_vector> transformedCorners(8);
    for (int i = 0; i < 8; ++i)
    {
        _vector corner = XMLoadFloat3(&XMFLOAT3(
            center.x + (i & 1 ? extents.x : -extents.x),
            center.y + (i & 2 ? extents.y : -extents.y),
            center.z + (i & 4 ? extents.z : -extents.z)
        ));
        transformedCorners[i] = XMVector3TransformCoord(corner, viewProjMatrix);
    }

    // 변환된 AABB 계산
    _vector vMin = transformedCorners[0], vMax = transformedCorners[0];
    for (int i = 1; i < 8; ++i)
    {
        vMin = XMVectorMin(vMin, transformedCorners[i]);
        vMax = XMVectorMax(vMax, transformedCorners[i]);
    }

    // 화면 공간 좌표 계산
    float minX = max(-1.f, min(1.f, XMVectorGetX(vMin)));
    float maxX = max(-1.f, min(1.f, XMVectorGetX(vMax)));
    float minY = max(-1.f, min(1.f, XMVectorGetY(vMin)));
    float maxY = max(-1.f, min(1.f, XMVectorGetY(vMax)));
    float minZ = XMVectorGetZ(vMin);
    float maxZ = XMVectorGetZ(vMax);

    // 화면 크기 동적 획득
    UINT screenWidth = 1280;
    UINT screenHeight = 720;

    // UV 좌표 계산
    _float2 minUV, maxUV;
    minUV.x = (minX + 1.0f) * 0.5f;
    minUV.y = (1.0f - maxY) * 0.5f;
    maxUV.x = (maxX + 1.0f) * 0.5f;
    maxUV.y = (1.0f - minY) * 0.5f;

    // HZB 밉맵 레벨 선택
    float boxSize = max(maxUV.x - minUV.x, maxUV.y - minUV.y) * max(screenWidth, screenHeight);
    UINT mipLevel = static_cast<UINT>(min(16, max(0, log2(boxSize))));

    // HZB를 사용한 오클루전 테스트
    float hzbDepth = m_pGameInstance->Sample_HZB(_float2((minUV.x + maxUV.x) * 0.5f, (minUV.y + maxUV.y) * 0.5f), mipLevel);

    return minZ <= hzbDepth;
}
float CCascade::SampleHZB(ID3D11ShaderResourceView* pHZBSRV, float x, float y, UINT level)
{
    // 이 함수는 HZB를 샘플링하는 간단한 구현입니다.
    // 실제 구현에서는 GPU를 사용하여 더 효율적으로 샘플링해야 합니다.

    // 텍스처에서 깊이 값을 읽어오는 코드를 여기에 구현해야 합니다.
    // 이 예제에서는 간단히 고정된 값을 반환합니다.
    return 1.0f;
}


void CCascade::Make_Planes(const _float4 * pPoints, _float4 * pPlanes)
{
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat4(&pPoints[1]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[6])));

	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat4(&pPoints[7]), XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[0])));
	
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[1])));

	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat4(&pPoints[3]), XMLoadFloat4(&pPoints[2]), XMLoadFloat4(&pPoints[6])));

	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[7])));

	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat4(&pPoints[0]), XMLoadFloat4(&pPoints[1]), XMLoadFloat4(&pPoints[2])));


	XMStoreFloat4(&pPlanes[6], XMPlaneFromPoints(XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[9]), XMLoadFloat4(&pPoints[10])));

	XMStoreFloat4(&pPlanes[7], XMPlaneFromPoints(XMLoadFloat4(&pPoints[11]), XMLoadFloat4(&pPoints[8]), XMLoadFloat4(&pPoints[4])));

	XMStoreFloat4(&pPlanes[8], XMPlaneFromPoints(XMLoadFloat4(&pPoints[8]), XMLoadFloat4(&pPoints[9]), XMLoadFloat4(&pPoints[5])));

	XMStoreFloat4(&pPlanes[9], XMPlaneFromPoints(XMLoadFloat4(&pPoints[7]), XMLoadFloat4(&pPoints[6]), XMLoadFloat4(&pPoints[10])));

	XMStoreFloat4(&pPlanes[10], XMPlaneFromPoints(XMLoadFloat4(&pPoints[9]), XMLoadFloat4(&pPoints[8]), XMLoadFloat4(&pPoints[11])));

	XMStoreFloat4(&pPlanes[11], XMPlaneFromPoints(XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[6])));


	XMStoreFloat4(&pPlanes[12], XMPlaneFromPoints(XMLoadFloat4(&pPoints[9]), XMLoadFloat4(&pPoints[13]), XMLoadFloat4(&pPoints[14])));

	XMStoreFloat4(&pPlanes[13], XMPlaneFromPoints(XMLoadFloat4(&pPoints[15]), XMLoadFloat4(&pPoints[12]), XMLoadFloat4(&pPoints[8])));

	XMStoreFloat4(&pPlanes[14], XMPlaneFromPoints(XMLoadFloat4(&pPoints[12]), XMLoadFloat4(&pPoints[13]), XMLoadFloat4(&pPoints[9])));

	XMStoreFloat4(&pPlanes[15], XMPlaneFromPoints(XMLoadFloat4(&pPoints[11]), XMLoadFloat4(&pPoints[10]), XMLoadFloat4(&pPoints[14])));

	XMStoreFloat4(&pPlanes[16], XMPlaneFromPoints(XMLoadFloat4(&pPoints[13]), XMLoadFloat4(&pPoints[12]), XMLoadFloat4(&pPoints[15])));

	XMStoreFloat4(&pPlanes[17], XMPlaneFromPoints(XMLoadFloat4(&pPoints[8]), XMLoadFloat4(&pPoints[9]), XMLoadFloat4(&pPoints[10])));
}

CCascade * CCascade::Create()
{
	CCascade*		pInstance = new CCascade();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CCascade");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CCascade::Free()
{
	Safe_Release(m_pGameInstance);
}

