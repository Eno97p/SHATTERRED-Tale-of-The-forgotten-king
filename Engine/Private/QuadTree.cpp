#include "..\Public\QuadTree.h"

#include "GameInstance.h"

CQuadTree::CQuadTree()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_iCorners[CORNER_LT] = iLT;
	m_iCorners[CORNER_RT] = iRT;
	m_iCorners[CORNER_RB] = iRB;
	m_iCorners[CORNER_LB] = iLB;

	if (1 == iRT - iLT)
		return S_OK;

	m_iCenterIndex = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RB]) * 0.5f;

	_uint		iLC, iTC, iRC, iBC;

	iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
	iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
	iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
	iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;	

	m_pChildren[CORNER_LT] = CQuadTree::Create(m_iCorners[CORNER_LT], iTC, m_iCenterIndex, iLC);
	m_pChildren[CORNER_RT] = CQuadTree::Create(iTC, m_iCorners[CORNER_RT], iRC, m_iCenterIndex);
	m_pChildren[CORNER_RB] = CQuadTree::Create(m_iCenterIndex, iRC, m_iCorners[CORNER_RB], iBC);
	m_pChildren[CORNER_LB] = CQuadTree::Create(iLC, m_iCenterIndex, iBC, m_iCorners[CORNER_LB]);
	
	return S_OK;
}

void CQuadTree::Culling(const _float4* pVertexPositions, _uint* pIndices, _uint* pNumIndices)
{
	if (nullptr == m_pChildren[CORNER_LT] || isDraw(pVertexPositions))
	{
		// 현재 노드를 그리는 로직
		if (m_pGameInstance->isIn_LocalFrustum(XMLoadFloat4(&pVertexPositions[m_iCenterIndex]), 5.f))
		{
			// 단순화된 삼각형 추가
			pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
			pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
			pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];

			pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
			pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
		}
	}
	else
	{
		// 자식 노드 순회
		for (size_t i = 0; i < CORNER_END; i++)
		{
			m_pChildren[i]->Culling(pVertexPositions, pIndices, pNumIndices);
		}
	}
}
void CQuadTree::Make_Neighbors()
{
	if (nullptr == m_pChildren[CORNER_LT]->m_pChildren[CORNER_LT])
		return;

	m_pChildren[CORNER_LT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChildren[CORNER_RT];
	m_pChildren[CORNER_LT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChildren[CORNER_LB];

	m_pChildren[CORNER_RT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChildren[CORNER_LT];
	m_pChildren[CORNER_RT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChildren[CORNER_RB];

	m_pChildren[CORNER_RB]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChildren[CORNER_LB];
	m_pChildren[CORNER_RB]->m_pNeighbors[NEIGHBOR_TOP] = m_pChildren[CORNER_RT];

	m_pChildren[CORNER_LB]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChildren[CORNER_RB];
	m_pChildren[CORNER_LB]->m_pNeighbors[NEIGHBOR_TOP] = m_pChildren[CORNER_LT];

	if (nullptr != m_pNeighbors[NEIGHBOR_RIGHT])
	{
		m_pChildren[CORNER_RT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChildren[CORNER_LT];
		m_pChildren[CORNER_RB]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChildren[CORNER_LB];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_BOTTOM])
	{
		m_pChildren[CORNER_LB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChildren[CORNER_LT];
		m_pChildren[CORNER_RB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChildren[CORNER_RT];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_LEFT])
	{
		m_pChildren[CORNER_LT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChildren[CORNER_RT];
		m_pChildren[CORNER_LB]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChildren[CORNER_RB];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_TOP])
	{
		m_pChildren[CORNER_LT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChildren[CORNER_LB];
		m_pChildren[CORNER_RT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChildren[CORNER_RB];
	}

	for (size_t i = 0; i < CORNER_END; i++)
		m_pChildren[i]->Make_Neighbors();	
}

_bool CQuadTree::isDraw(const _float4* pVertexPositions)
{
	_vector		vCamPosition = m_pGameInstance->Get_CamPosition();

	_float		fDistance = XMVectorGetX(XMVector3Length(vCamPosition - XMLoadFloat4(&pVertexPositions[m_iCenterIndex])));

	_float		fWidth = m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT];

	if (fDistance * 0.1f > fWidth)
		return true;

	return false;
}

CQuadTree * CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree*		pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB)))
	{
		MSG_BOX("Failed To Created : CQuadTree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuadTree::Free()
{
	for (size_t i = 0; i < CORNER_END; i++)	
		Safe_Release(m_pChildren[i]);

	Safe_Release(m_pGameInstance);
}
