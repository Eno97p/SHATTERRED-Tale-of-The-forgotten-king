#include "..\Public\Frustum.h"
#include "GameInstance.h"

CFrustum::CFrustum()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Initialize()
{
	m_vPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vPoints[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Update()
{
	_matrix ViewMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);
	_matrix ProjMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ);

	_float4	vPoints[8] = {};

	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat4(&vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), ProjMatrixInverse));		
	}
	for (size_t i = 0; i < 8; i++)
		XMStoreFloat4(&m_vWorldPoints[i], XMVector3Transform(XMLoadFloat4(&vPoints[i]), ViewMatrixInverse));

	Make_Planes(m_vWorldPoints, m_vWorldPlanes);

	m_fTotalTime += m_pGameInstance->Get_TimeDelta(TEXT("Timer_Default"));

}

void CFrustum::Transform_ToLocalSpace(_fmatrix WorldMatrixInv)
{
	_float4		vLocalPoints[8] = {};

	for (size_t i = 0; i < 8; i++)
		XMStoreFloat4(&vLocalPoints[i], XMVector3Transform(XMLoadFloat4(&m_vWorldPoints[i]), WorldMatrixInv));

	Make_Planes(vLocalPoints, m_vLocalPlanes);
}

_bool CFrustum::isIn_WorldFrustum(_fvector vPosition, _float fRange)
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


_bool CFrustum::isIn_LocalFrustum(_fvector vPosition, _float fRange)
{
	for (size_t i = 0; i < 6; i++)
	{		
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vLocalPlanes[i]), vPosition)))
			return false;
	}
	return true;
}

bool CFrustum::isVisible(_vector vPos, PxActor* actor)
{
    // �Ÿ� ��� �ø�
    _vector vCamPos = m_pGameInstance->Get_CamPosition();
    float distanceToCamera = XMVector3Length(XMVectorSubtract(vPos, vCamPos)).m128_f32[0];
    if (distanceToCamera > m_maxVisibleDistance)
        return false;

    // �ٿ�� �ڽ� ���� ���
    PxBounds3 bounds = actor->getWorldBounds();
    PxVec3 center = bounds.getCenter();
    PxVec3 extents = bounds.getExtents();

    // �ٿ�� ���� ����� �������� �ø�
    float radius = extents.magnitude();
    if (!isIn_WorldFrustum(XMLoadFloat3(&XMFLOAT3(center.x, center.y, center.z)), radius))
        return false;

    // �� �������� ��ȯ
    _matrix viewProjMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW) *
        m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);

    // �ٿ�� �ڽ��� 8�� �ڳʸ� ��ȯ
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

    // ��ȯ�� AABB ���
    _vector vMin = transformedCorners[0], vMax = transformedCorners[0];
    for (int i = 1; i < 8; ++i)
    {
        vMin = XMVectorMin(vMin, transformedCorners[i]);
        vMax = XMVectorMax(vMax, transformedCorners[i]);
    }

    // ȭ�� ���� ��ǥ ���
    float minX = max(-1.f, min(1.f, XMVectorGetX(vMin)));
    float maxX = max(-1.f, min(1.f, XMVectorGetX(vMax)));
    float minY = max(-1.f, min(1.f, XMVectorGetY(vMin)));
    float maxY = max(-1.f, min(1.f, XMVectorGetY(vMax)));
    float minZ = XMVectorGetZ(vMin);
    float maxZ = XMVectorGetZ(vMax);

    // ȭ�� ũ�� ���� ȹ��
    UINT screenWidth = 1280;
    UINT screenHeight = 720;

    // UV ��ǥ ���
    _float2 minUV, maxUV;
    minUV.x = (minX + 1.0f) * 0.5f;
    minUV.y = (1.0f - maxY) * 0.5f;
    maxUV.x = (maxX + 1.0f) * 0.5f;
    maxUV.y = (1.0f - minY) * 0.5f;

    // HZB �Ӹ� ���� ����
    float boxSize = max(maxUV.x - minUV.x, maxUV.y - minUV.y) * max(screenWidth, screenHeight);
    UINT mipLevel = static_cast<UINT>(min(16, max(0, log2(boxSize))));

    // HZB�� ����� ��Ŭ���� �׽�Ʈ
    float hzbDepth = m_pGameInstance->Sample_HZB(_float2((minUV.x + maxUV.x) * 0.5f, (minUV.y + maxUV.y) * 0.5f), mipLevel);

    return minZ <= hzbDepth;
}
float CFrustum::SampleHZB(ID3D11ShaderResourceView* pHZBSRV, float x, float y, UINT level)
{
    // �� �Լ��� HZB�� ���ø��ϴ� ������ �����Դϴ�.
    // ���� ���������� GPU�� ����Ͽ� �� ȿ�������� ���ø��ؾ� �մϴ�.

    // �ؽ�ó���� ���� ���� �о���� �ڵ带 ���⿡ �����ؾ� �մϴ�.
    // �� ���������� ������ ������ ���� ��ȯ�մϴ�.
    return 1.0f;
}


void CFrustum::Make_Planes(const _float4 * pPoints, _float4 * pPlanes)
{
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat4(&pPoints[1]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[6])));

	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat4(&pPoints[7]), XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[0])));
	
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[1])));

	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat4(&pPoints[3]), XMLoadFloat4(&pPoints[2]), XMLoadFloat4(&pPoints[6])));

	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[7])));

	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat4(&pPoints[0]), XMLoadFloat4(&pPoints[1]), XMLoadFloat4(&pPoints[2])));
}

CFrustum * CFrustum::Create()
{
	CFrustum*		pInstance = new CFrustum();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CFrustum");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CFrustum::Free()
{
	Safe_Release(m_pGameInstance);
}

