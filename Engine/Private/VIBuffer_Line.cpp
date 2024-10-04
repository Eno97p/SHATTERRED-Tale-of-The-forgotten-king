#include "VIBuffer_Line.h"

CVIBuffer_Line::CVIBuffer_Line(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Line::CVIBuffer_Line(const CVIBuffer_Line & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Line::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Line::Initialize(void * pArg)
{
	
	LineDesc = make_shared<LINE_DESC>(*((LINE_DESC*)pArg));

	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	m_iIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXPOS);
	m_iNumVertices = 2;

	m_iIndexStride = 2;
	m_iNumIndices = 2;

#pragma region VERTEX_BUFFER 

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	pVertices[0].vPosition = LineDesc->vPos[0];
	pVertices[1].vPosition = LineDesc->vPos[1];

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER 
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

CVIBuffer_Line * CVIBuffer_Line::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Line*		pInstance = new CVIBuffer_Line(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CVIBuffer_Line");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Line::Clone(void * pArg)
{
	CVIBuffer_Line*		pInstance = new CVIBuffer_Line(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Line");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CVIBuffer_Line::Free()
{
	__super::Free();
}
