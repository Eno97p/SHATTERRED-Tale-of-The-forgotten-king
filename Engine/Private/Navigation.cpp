#include "..\Public\Navigation.h"

#include "Cell.h"
#include "Shader.h"
#include "GameInstance.h"
#include "VIBuffer_Cell.h"

_float4x4	CNavigation::m_WorldMatrix{};

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent { pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent{ rhs }
	, m_Cells{rhs.m_Cells}	
#ifdef _DEBUG
	, m_pShader{rhs.m_pShader}
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif
}

HRESULT CNavigation::Initialize_Prototype(const wstring & strNavigationDataFile)
{
	_ulong		dwByte = {};
	HANDLE		hFile = CreateFile(strNavigationDataFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoint[CCell::POINT_END] = {};

	while (true)
	{
		ReadFile(hFile, vPoint, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CCell*		pCell = CCell::Create(m_pDevice, m_pContext, vPoint, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.emplace_back(pCell);
	}

	CloseHandle(hFile);

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

#ifdef _DEBUG	
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return S_OK;

	m_iCurrentCellIndex = static_cast<NAVIGATION_DESC*>(pArg)->iCurrentCellIndex;

	return S_OK;
}

void CNavigation::Update(const _float4x4 * pWorldMatrix)
{
	m_WorldMatrix = *pWorldMatrix;
}

_bool CNavigation::isMove(_fvector vPosition)
{
	_vector		vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));


	_int			iNeighborIndex = { -1 };

	/* 셀 안에서 움직였다. */
	if (true == m_Cells[m_iCurrentCellIndex]->isIn(vLocalPos, &iNeighborIndex))
		return true;

	/* 셀 밖으로 움직였다. */
	else
	{
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vLocalPos, &iNeighborIndex))
				{
					m_iCurrentCellIndex = iNeighborIndex;
					return true;
				}				
			}
			
		}
		else
			return false;
	}
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);

	m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ));

	if (-1 == m_iCurrentCellIndex)
	{
		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4))))
			return E_FAIL;

		m_pShader->Begin(0);

		for (auto& pCell : m_Cells)
			pCell->Render();

		return S_OK;
	}

	else
	{
		_float4x4	Matrix = m_WorldMatrix;

		Matrix._42 += 0.1f;

		m_pShader->Bind_Matrix("g_WorldMatrix", &Matrix);

		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &_float4(1.f, 0.0f, 0.f, 1.f), sizeof(_float4))))
			return E_FAIL;

		m_pShader->Begin(0);

		m_Cells[m_iCurrentCellIndex]->Render();

		return S_OK;
	}




}

#endif

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
				pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDestCell);

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
				pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDestCell);

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
				pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDestCell);
		}
	}
	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strNavigationDataFile)
{
	CNavigation*		pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strNavigationDataFile)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		return nullptr;
	}

	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*		pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavigation");
		return nullptr;
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif
}
