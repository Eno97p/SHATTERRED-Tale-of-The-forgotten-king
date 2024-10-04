#include "VIBuffer_SwordTrail.h"

CVIBuffer_SwordTrail::CVIBuffer_SwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_SwordTrail::CVIBuffer_SwordTrail(const CVIBuffer_SwordTrail& rhs)
	: CVIBuffer{ rhs }
{
}

HRESULT CVIBuffer_SwordTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_SwordTrail::Initialize(void* pArg)
{
	m_pDesc =  make_shared<SwordTrailDesc>(*((SwordTrailDesc*)pArg));
	m_iNumInstance = m_pDesc->iNumInstance;

	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_iIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iNumVertexBuffers = 2;
	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;

	m_iIndexStride = 2;
	m_iIndexCountPerInstance = 1;
	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;
	m_iInstanceStride = sizeof(SwordTrailVertex);

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

	SwordTrailVertex* pInstanceVertices = new SwordTrailVertex[m_iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(SwordTrailVertex) * m_iNumInstance);

	_matrix ParentMat = XMLoadFloat4x4(m_pDesc->ParentMat);
	_vector vUp = XMVector4Normalize(ParentMat.r[1]);
	_vector vPos = XMVector3TransformCoord(XMLoadFloat3(&m_pDesc->vOffsetPos), ParentMat);

	_vector Top = vPos + vUp * m_pDesc->vSize;
	_vector Bottom = vPos - vUp * m_pDesc->vSize;

	m_pTrailVertex.emplace_back(make_pair(Top, Bottom));

	//Zero = 왼쪽 위, One 왼쪽 아래, Two 오른쪽 아래, Three 오른쪽 위 
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		XMStoreFloat3(&pInstanceVertices[i].Zero, Top);
		XMStoreFloat3(&pInstanceVertices[i].One, Bottom);
		XMStoreFloat3(&pInstanceVertices[i].Two, Bottom);
		XMStoreFloat3(&pInstanceVertices[i].Three, Top);

		pInstanceVertices[i].texCoord0 = _float2(_float(i) / _float(m_iNumInstance), 0.f);
		pInstanceVertices[i].texCoord1 = _float2(_float(i) / _float(m_iNumInstance), 1.f);
		pInstanceVertices[i].texCoord2 = _float2(_float(i+1) / _float(m_iNumInstance),1.f);
		pInstanceVertices[i].texCoord3 = _float2(_float(i+1) / _float(m_iNumInstance), 0.f);

		pInstanceVertices[i].lifetime.x = m_pDesc->fLifeTime;
		pInstanceVertices[i].lifetime.y = 0.f;
	}

	m_InitialData.pSysMem = pInstanceVertices;
	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InitialData, &m_pVBInstance)))
		return E_FAIL;
	Safe_Delete_Array(pInstanceVertices);

#pragma endregion
	return S_OK;
}

HRESULT CVIBuffer_SwordTrail::Bind_Buffers()
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

HRESULT CVIBuffer_SwordTrail::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);
	return S_OK;
}

void CVIBuffer_SwordTrail::Tick(_float fDelta)
{
	XMMATRIX ParentMat = XMLoadFloat4x4(m_pDesc->ParentMat);
	XMVECTOR vUp = XMVector4Normalize(ParentMat.r[1]);

	XMVECTOR vPos = XMVector3TransformCoord(XMLoadFloat3(&m_pDesc->vOffsetPos), ParentMat);
	XMVECTOR Top = vPos + vUp * m_pDesc->vSize;
	XMVECTOR Bottom = vPos - vUp * m_pDesc->vSize;

	m_pTrailVertex.emplace_back(make_pair(Top, Bottom));

	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	SwordTrailVertex* pVertices = (SwordTrailVertex*)SubResource.pData;
	for (_int i = m_iNumInstance-1; i >= 0; i--)
	{
		if (i == 0)
		{
			pVertices[i].Three = pVertices[i].Zero;
			pVertices[i].Two = pVertices[i].One;
			XMStoreFloat3(&pVertices[i].Zero, Top);
			XMStoreFloat3(&pVertices[i].One, Bottom);
		}
		else if (i == m_iNumInstance - 1)
		{
			XMStoreFloat3(&pVertices[i].Three,m_pTrailVertex[0].first);
			XMStoreFloat3(&pVertices[i].Two,m_pTrailVertex[0].second);
			XMStoreFloat3(&pVertices[i].Zero,m_pTrailVertex[0].first);
			XMStoreFloat3(&pVertices[i].One,m_pTrailVertex[0].second);
		}
		else
		{
			pVertices[i].Three = pVertices[i].Zero;
			pVertices[i].Two = pVertices[i].One;
			pVertices[i].Zero = pVertices[i - 1].Three;
			pVertices[i].One = pVertices[i - 1].Two;
		}
	}
	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_SwordTrail::Tick_AI(_float fDelta)
{
	bool allInstancesDead = true;
	XMMATRIX ParentMat = XMLoadFloat4x4(m_pDesc->ParentMat);
	XMVECTOR vUp = XMVector4Normalize(ParentMat.r[1]);

	XMVECTOR vPos = XMVector3TransformCoord(XMLoadFloat3(&m_pDesc->vOffsetPos), ParentMat);
	XMVECTOR Top = vPos + vUp * m_pDesc->vSize;
	XMVECTOR Bottom = vPos - vUp * m_pDesc->vSize;

	XMFLOAT3 topPoint, bottomPoint;
	XMStoreFloat3(&topPoint, Top);
	XMStoreFloat3(&bottomPoint, Bottom);

	m_TopPoints.push_back(topPoint);
	m_BottomPoints.push_back(bottomPoint);

	if (m_TopPoints.size() > m_iNumInstance)
	{
		m_TopPoints.erase(m_TopPoints.begin());
		m_BottomPoints.erase(m_BottomPoints.begin());
	}

	D3D11_MAPPED_SUBRESOURCE SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	SwordTrailVertex* pVertices = (SwordTrailVertex*)SubResource.pData;

	size_t pointCount = m_TopPoints.size();

	for (_int i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].lifetime.y += fDelta;

		if (pVertices[i].lifetime.y < m_pDesc->fMaxTime)
		{
			float t = static_cast<float>(i) / (m_iNumInstance - 1);
			float nextT = static_cast<float>(i + 1) / (m_iNumInstance - 1);

			XMVECTOR topPos, bottomPos, nextTopPos, nextBottomPos;

			if (pointCount < 2)
			{
				topPos = nextTopPos = Top;
				bottomPos = nextBottomPos = Bottom;
			}
			else if (pointCount < 4)
			{
				XMVECTOR topStart = XMLoadFloat3(&m_TopPoints.front());
				XMVECTOR topEnd = XMLoadFloat3(&m_TopPoints.back());
				XMVECTOR bottomStart = XMLoadFloat3(&m_BottomPoints.front());
				XMVECTOR bottomEnd = XMLoadFloat3(&m_BottomPoints.back());

				topPos = XMVectorLerp(topStart, topEnd, t);
				bottomPos = XMVectorLerp(bottomStart, bottomEnd, t);
				nextTopPos = XMVectorLerp(topStart, topEnd, nextT);
				nextBottomPos = XMVectorLerp(bottomStart, bottomEnd, nextT);
			}
			else
			{
				size_t index = min(static_cast<size_t>(t * (pointCount - 1)), pointCount - 1);
				size_t p1 = min(max(index, (size_t)1), pointCount - 2);

				XMVECTOR v0 = XMLoadFloat3(&m_TopPoints[p1 - 1]);
				XMVECTOR v1 = XMLoadFloat3(&m_TopPoints[p1]);
				XMVECTOR v2 = XMLoadFloat3(&m_TopPoints[p1 + 1]);
				XMVECTOR v3 = p1 + 2 < pointCount ? XMLoadFloat3(&m_TopPoints[p1 + 2]) : v2;

				float localT = t * (pointCount - 1) - p1;
				float localNextT = nextT * (pointCount - 1) - p1;

				topPos = CatmullRom(v0, v1, v2, v3, localT);
				nextTopPos = CatmullRom(v0, v1, v2, v3, localNextT);

				v0 = XMLoadFloat3(&m_BottomPoints[p1 - 1]);
				v1 = XMLoadFloat3(&m_BottomPoints[p1]);
				v2 = XMLoadFloat3(&m_BottomPoints[p1 + 1]);
				v3 = p1 + 2 < pointCount ? XMLoadFloat3(&m_BottomPoints[p1 + 2]) : v2;

				bottomPos = CatmullRom(v0, v1, v2, v3, localT);
				nextBottomPos = CatmullRom(v0, v1, v2, v3, localNextT);
			}

			XMStoreFloat3(&pVertices[i].Zero, topPos);
			XMStoreFloat3(&pVertices[i].One, bottomPos);
			XMStoreFloat3(&pVertices[i].Two, nextBottomPos);
			XMStoreFloat3(&pVertices[i].Three, nextTopPos);


		}


		if (pVertices[i].lifetime.y >= pVertices[i].lifetime.x)
		{
			pVertices[i].lifetime.y = pVertices[i].lifetime.x;
		}
		if (pVertices[i].lifetime.y < pVertices[i].lifetime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	m_bInstanceDead = allInstancesDead;
}

void CVIBuffer_SwordTrail::Tick_AI_UPgrade(_float fDelta)
{
	XMMATRIX ParentMat = XMLoadFloat4x4(m_pDesc->ParentMat);
	XMVECTOR vUp = XMVector4Normalize(ParentMat.r[1]);

	XMVECTOR vPos = XMVector3TransformCoord(XMLoadFloat3(&m_pDesc->vOffsetPos), ParentMat);
	XMVECTOR Top = vPos + vUp * m_pDesc->vSize;
	XMVECTOR Bottom = vPos - vUp * m_pDesc->vSize;

	XMFLOAT3 topPoint, bottomPoint;
	XMStoreFloat3(&topPoint, Top);
	XMStoreFloat3(&bottomPoint, Bottom);

	m_TopPoints.push_back(topPoint);
	m_BottomPoints.push_back(bottomPoint);

	if (m_TopPoints.size() > m_iNumInstance)
	{
		m_TopPoints.erase(m_TopPoints.begin());
		m_BottomPoints.erase(m_BottomPoints.begin());
	}

	D3D11_MAPPED_SUBRESOURCE SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	SwordTrailVertex* pVertices = (SwordTrailVertex*)SubResource.pData;

	size_t pointCount = m_TopPoints.size();



}

XMVECTOR CVIBuffer_SwordTrail::CatmullRom(XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, XMVECTOR v3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;

	XMVECTOR a = v1;
	XMVECTOR b = 0.5f * (-v0 + v2);
	XMVECTOR c = 0.5f * (2.0f * v0 - 5.0f * v1 + 4.0f * v2 - v3);
	XMVECTOR d = 0.5f * (-v0 + 3.0f * v1 - 3.0f * v2 + v3);

	return a + b * t + c * t2 + d * t3;
}


CVIBuffer_SwordTrail* CVIBuffer_SwordTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_SwordTrail* pInstance = new CVIBuffer_SwordTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CVIBuffer_SwordTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_SwordTrail::Clone(void* pArg)
{
	CVIBuffer_SwordTrail* pInstance = new CVIBuffer_SwordTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_SwordTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_SwordTrail::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}
