#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct BOUNDING_AABB_DESC : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents;
	}BOUNDING_AABB_DESC;
private:
	CBounding_AABB();
	virtual ~CBounding_AABB() = default;

public:
	virtual void* Get_Desc() override { 
		return m_pDesc;
	}

public:
	HRESULT Initialize(const void* pArg);
	virtual void Tick(_fmatrix WorldMatrix) override;
	virtual _bool Intersect(CCollider::TYPE eTargetType, CBounding* pTargetBounding) override;
	virtual _bool IntersectRay(_vector vRayOrigin, _vector vRayDirection, _float fDistance) override;
	virtual _float3 Get_Center();
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif

private:
	BoundingBox*			m_pOriginalDesc = { nullptr };
	BoundingBox*			m_pDesc = { nullptr };

private:
	_bool Intersect_AABB(const BoundingBox* pTargetBounding);
	
	_float3 Compute_Min(const BoundingBox* pTargetBounding);
	_float3 Compute_Max(const BoundingBox* pTargetBounding);

public:
	static CBounding_AABB* Create(const void* pArg);
	virtual void Free() override;
};

END