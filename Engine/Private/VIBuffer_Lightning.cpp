#include "VIBuffer_Lightning.h"

CVIBuffer_Lightning::CVIBuffer_Lightning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Lightning::CVIBuffer_Lightning(const CVIBuffer_Lightning& rhs)
	: CVIBuffer{ rhs }
{
}

HRESULT CVIBuffer_Lightning::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Lightning::Initialize(void* pArg)
{
	m_RandomNumber = mt19937_64(m_RandomDevice());
	m_pDesc =  make_shared<LIGHTNINGDESC>(*((LIGHTNINGDESC*)pArg));
	m_iNumInstance = m_pDesc->iNumInstance;

	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	m_iIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iNumVertexBuffers = 2;
	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;

	m_iIndexStride = 2;
	m_iIndexCountPerInstance = m_pDesc->NumSegments[1] + 1;;
	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;
	m_iInstanceStride = sizeof(VTXELECTRON);

#pragma region VERTEX_BUFFER 

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.f);
	pVertices[0].vPSize = _float2(1.f, 1.f);

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
	for (size_t i = 0; i < m_iNumInstance; ++i)
	{
		for (size_t j = 0; j < m_iIndexCountPerInstance; ++j)
		{
			pIndices[i * m_iIndexCountPerInstance + j] = j;
		}
	}
	m_InitialData.pSysMem = pIndices;
	
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);

#pragma endregion
#pragma region INSTANCE_BUFFER

	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	VTXELECTRON* pInstanceVertices = new VTXELECTRON[m_iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXELECTRON) * m_iNumInstance);

	uniform_real_distribution<float> LifeTime(m_pDesc->fLifeTime.x, m_pDesc->fLifeTime.y);
	uniform_real_distribution<float> Tickness(m_pDesc->fThickness.x, m_pDesc->fThickness.y);
	uniform_real_distribution<float> Amplitude(m_pDesc->Amplitude.x, m_pDesc->Amplitude.y);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pInstanceVertices[i].vPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);  // 초기 위치
		pInstanceVertices[i].vPSize = XMFLOAT2(1.0f, 1.0f);  // 기본 크기
		pInstanceVertices[i].vStartpos = m_pDesc->vStartpos;
		pInstanceVertices[i].vEndpos = m_pDesc->vEndpos;
		pInstanceVertices[i].vLifeTime.x = LifeTime(m_RandomNumber);
		pInstanceVertices[i].vLifeTime.y = 0.f;
		pInstanceVertices[i].fThickness = Tickness(m_RandomNumber); 
		pInstanceVertices[i].fAmplitude = Amplitude(m_RandomNumber);
		pInstanceVertices[i].iNumSegments = RandomInt(m_pDesc->NumSegments[0], m_pDesc->NumSegments[1]);
		if (pInstanceVertices[i].iNumSegments > 56)
			pInstanceVertices[i].iNumSegments = 56;
	}

	m_InitialData.pSysMem = pInstanceVertices;
	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InitialData, &m_pVBInstance)))
		return E_FAIL;
	Safe_Delete_Array(pInstanceVertices);

#pragma endregion
	return S_OK;
}

HRESULT CVIBuffer_Lightning::Bind_Buffers()
{
	ID3D11Buffer* pVertexBuffers[] = {
	m_pVB,
	m_pVBInstance
	};

	_uint					iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	_uint					iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_iIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

HRESULT CVIBuffer_Lightning::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);
	return S_OK;
}

void CVIBuffer_Lightning::Tick(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXELECTRON* pVertices = (VTXELECTRON*)SubResource.pData;
	for (_int i = 0; i < m_iNumInstance; ++i)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;

		_float3 vTop = pVertices[i].vStartpos;
		_float3 vBottom = pVertices[i].vEndpos;


		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}
	m_pContext->Unmap(m_pVBInstance, 0);

	m_bInstanceDead = allInstancesDead;
}


CVIBuffer_Lightning* CVIBuffer_Lightning::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Lightning* pInstance = new CVIBuffer_Lightning(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CVIBuffer_Lightning");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Lightning::Clone(void* pArg)
{
	CVIBuffer_Lightning* pInstance = new CVIBuffer_Lightning(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Lightning");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Lightning::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}
