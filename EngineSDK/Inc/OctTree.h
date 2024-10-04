#pragma once

#include "Base.h"
#include "GameObject.h"
#include "Frustum.h"

BEGIN(Engine)

class COctTree final : public CBase
{
public:
	enum CORNER {
		TOP_LEFT_FRONT, TOP_RIGHT_FRONT, BOTTOM_RIGHT_FRONT, BOTTOM_LEFT_FRONT,
		TOP_LEFT_BACK, TOP_RIGHT_BACK, BOTTOM_RIGHT_BACK, BOTTOM_LEFT_BACK,
		CORNER_END
	};

	enum NEIGHBOR {
		NEIGHBOR_LEFT, NEIGHBOR_RIGHT,
		NEIGHBOR_TOP, NEIGHBOR_BOTTOM,
		NEIGHBOR_FRONT, NEIGHBOR_BACK,
		NEIGHBOR_END
	};
private:
	COctTree();
	HRESULT Initialize(const _float3& vMin, const _float3& vMax, _int depth);
	virtual ~COctTree() = default;

public:
	void Culling(const _float4* pVertexPositions, _uint* pIndices, _uint* pNumIndices);
	void Make_Neighbors();
	//void Update_OctTree();
	//void UpdateNodeVisibility();


	//void AddObject(CGameObject* pObject, PxActor* pActor);
	//bool IsOverlapping(const _float3& min, const _float3& max);
	//bool IsObjectVisible(CGameObject* pObject);
	//bool IsNodeVisible();
	//bool IsNodeOccluded();


private:
	COctTree* m_pChildren[CORNER_END] = { nullptr };
	COctTree* m_pNeighbors[NEIGHBOR_END] = { nullptr, nullptr, nullptr, nullptr };
	_uint					m_iCenterIndex = { 0 };
	_uint					m_iCorners[CORNER_END] = { 0 };
	class CGameInstance* m_pGameInstance = { nullptr };

	_float3     m_vMin, m_vMax;  // ��� �ڽ�

	vector<CGameObject*> m_Objects;

	PxVec3 m_CamPos = { 0.f, 0.f, 0.f };
private:
	int m_iLODLevel = 0;                    // �ʱ� LOD ����
	float m_fMinLODDistance = 100.0f;       // �ּ� LOD �Ÿ� (100 ����)
	float m_fLODDistanceMultiplier = 2.0f;  // LOD �Ÿ� ���� ����

	_float3 m_vCorners[CORNER_END];                  // ����� 8�� �𼭸� ��ġ
	_uint m_iTerrainIndices[CORNER_END];  // ���� �޽ÿ����� �ε���

	const int MAX_DEPTH = 6;                // �ִ� ���� ����
	const float MIN_NODE_SIZE = 10.0f;      // �ּ� ��� ũ�� (10 ����)
private:
	//bool IsNodeVisible(CFrustum* frustum);
	_bool isDraw(const _float4* pVertexPositions);
	_bool isDraw(const _float3& vCenter);

	void SetLODParameters(_float minLODDistance = 100.0f, _float lodDistanceMultiplier = 2.0f);


public:
	//static COctTree* Create(_uint iTLF, _uint iTRF, _uint iBRF, _uint iBLF, _uint iTLB, _uint iTRB, _uint iBRB, _uint iBLB);
	static COctTree* Create(const _float3& vMin, const _float3& vMax, _int depth);
	virtual void Free() override;
};

END