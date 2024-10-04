#include "..\Public\Bounding_OBB.h"
#include "DebugDraw.h"

CBounding_OBB::CBounding_OBB()
{
}

HRESULT CBounding_OBB::Initialize(const void * pArg)
{
	const BOUNDING_OBB_DESC*		pDesc = static_cast<const BOUNDING_OBB_DESC*>(pArg);

	_float4		vQuaternion;

	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));

	m_pOriginalDesc = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vQuaternion);
	m_pDesc = new BoundingOrientedBox(*m_pOriginalDesc);

	return S_OK;
}

void CBounding_OBB::Tick(_fmatrix WorldMatrix)
{	
	m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
}

_bool CBounding_OBB::Intersect(CCollider::TYPE eTargetType, CBounding* pTargetBounding)
{
	_bool			isColl{ false };

	switch (eTargetType)
	{
	case CCollider::TYPE_AABB:
		isColl = m_pDesc->Intersects(*(static_cast<BoundingBox*>(pTargetBounding->Get_Desc())));
		break;
	case CCollider::TYPE_OBB:
		isColl = m_pDesc->Intersects(*(static_cast<BoundingOrientedBox*>(pTargetBounding->Get_Desc())));
		break;
	case CCollider::TYPE_SPHERE:
		isColl = m_pDesc->Intersects(*(static_cast<BoundingSphere*>(pTargetBounding->Get_Desc())));
		break;
	}
	
	return isColl;
}

_bool CBounding_OBB::IntersectRay(_vector vRayOrigin, _vector vRayDirection, _float fDistance)
{
	_bool			isColl{ false };
	isColl = m_pDesc->Intersects(vRayOrigin, vRayDirection, fDistance);

	return isColl;
}

_float3 CBounding_OBB::Get_Center()
{
	return m_pDesc->Center;
}

#ifdef _DEBUG

HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pDesc, vColor);

	return S_OK;
}

#endif

_bool CBounding_OBB::Intersect_OBB(const BoundingOrientedBox * pTargetBounding)
{
	OBB_DESC			OBBDesc[2] = { Compute_OBBDesc(m_pDesc), Compute_OBBDesc(pTargetBounding) };

	_float				fDistance[3] = { 0 };

	for (size_t i = 0; i < 2; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			/* »¡°£»ö±æÀÌ. */
			_vector			vCenterDir = XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter);
			fDistance[0] = fabs(XMVectorGetX(XMVector3Dot(vCenterDir, XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			fDistance[1] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[0]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[1]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterDir[2]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			fDistance[2] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[0]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[1]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterDir[2]), XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			if (fDistance[1] + fDistance[2] < fDistance[0])
				return false;
		}
	}
	return true;
}

CBounding_OBB::OBB_DESC CBounding_OBB::Compute_OBBDesc(const BoundingOrientedBox * pDesc)
{
	OBB_DESC		OBBDesc{};

	_float3		vPoints[8] = {};

	pDesc->GetCorners(vPoints);

	OBBDesc.vCenter = pDesc->Center;

	XMStoreFloat3(&OBBDesc.vAlignDir[0], XMVector3Normalize(XMLoadFloat3(&vPoints[5]) - XMLoadFloat3(&vPoints[4])));
	XMStoreFloat3(&OBBDesc.vAlignDir[1], XMVector3Normalize(XMLoadFloat3(&vPoints[7]) - XMLoadFloat3(&vPoints[4])));
	XMStoreFloat3(&OBBDesc.vAlignDir[2], XMVector3Normalize(XMLoadFloat3(&vPoints[0]) - XMLoadFloat3(&vPoints[4])));

	XMStoreFloat3(&OBBDesc.vCenterDir[0], XMLoadFloat3(&OBBDesc.vAlignDir[0]) * pDesc->Extents.x);
	XMStoreFloat3(&OBBDesc.vCenterDir[1], XMLoadFloat3(&OBBDesc.vAlignDir[1]) * pDesc->Extents.y);
	XMStoreFloat3(&OBBDesc.vCenterDir[2], XMLoadFloat3(&OBBDesc.vAlignDir[2]) * pDesc->Extents.z);	

	return OBBDesc;
}

CBounding_OBB * CBounding_OBB::Create(const void * pArg)
{
	CBounding_OBB*		pInstance = new CBounding_OBB();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Created : CBounding_OBB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_OBB::Free()
{	
	__super::Free();

	Safe_Delete(m_pOriginalDesc);
	Safe_Delete(m_pDesc);
}

