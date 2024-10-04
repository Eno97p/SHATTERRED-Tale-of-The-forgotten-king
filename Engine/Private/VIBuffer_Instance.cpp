#include "..\Public\VIBuffer_Instance.h"
#include "VIBuffer_Terrain.h"

#include"GameInstance.h"
#include "ComputeShader_Buffer.h"

#include"CCuda.h"
CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& rhs)
	: CVIBuffer{ rhs }
	, m_iNumInstance{ rhs.m_iNumInstance }
	, m_iInstanceStride{ rhs.m_iInstanceStride }
	, m_iIndexCountPerInstance{ rhs.m_iIndexCountPerInstance }
	, m_InstanceBufferDesc{ rhs.m_InstanceBufferDesc }
	, m_pSpeeds{ rhs.m_pSpeeds }
	, m_pOriginalSpeed{ rhs.m_pOriginalSpeed }
	, m_pOriginalPositions{ rhs.m_pOriginalPositions }
	, m_pOriginalGravity{ rhs.m_pOriginalGravity }
	, m_InstanceDesc{ rhs.m_InstanceDesc }
	, m_bInstanceDead{ rhs.m_bInstanceDead }
{
	
	//m_pContext->CopyResource(m_pVBInstance, rhs.m_pVBInstance);
}

HRESULT CVIBuffer_Instance::Initialize_Prototype(const INSTANCE_DESC& InstanceDesc)
{
	m_iNumInstance = InstanceDesc.iNumInstance;

	m_RandomNumber = mt19937_64(m_RandomDevice());



	
	
	//cudaMalloc(&d_pInstanceData, sizeof(VTXMATRIX) * m_iNumInstance);		//메모리 할당		
	//cudaMalloc(&d_visibleCount, sizeof(int));								//메모리 할당
	//초기화

	return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void* pArg)
{

	// CVIBuffer_Instance 클래스 내부
	//CComputeShader_Buffer* m_pCullingCS = nullptr;

	//// 초기화 함수에서
	//m_pCullingCS = CComputeShader_Buffer::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/CullingCS.hlsl", "CS_Cull");
	//if (nullptr == m_pCullingCS)
	//	return E_FAIL;

	//CComputeShader_Buffer::CSBUFFER_DESC cullingDesc;
	//cullingDesc.numInputVariables = 1;
	//cullingDesc.arg = (void**)&m_pVertexInstanceBuffer;
	//cullingDesc.variableSize = sizeof(VTXMATRIX);
	//cullingDesc.variableNum = m_iNumInstance;
	//cullingDesc.variableFormat = DXGI_FORMAT_UNKNOWN;

	//if (FAILED(m_pCullingCS->Initialize(&cullingDesc)))
	//	return E_FAIL;




	return S_OK;
}

HRESULT CVIBuffer_Instance::Bind_Buffers()
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
HRESULT CVIBuffer_Instance::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}


void CVIBuffer_Instance::Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] -= fTimeDelta * 0.1f;
		if (m_pSize[i] < 0.f)
			m_pSize[i] = 0.f;
		
		pVertices[i].vGravity = m_pOriginalGravity[i] * (pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x);
		XMVECTOR			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc.vOffsetPos), 0.f);
		vDir -= XMVectorSet(0.f, pVertices[i].vGravity, 0.f, 0.f);
	
		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
	
		XMMATRIX		RotationMatrix = XMMatrixRotationAxis(vRight, XMConvertToRadians(360) * fTimeDelta);
		vRight = XMVector3TransformNormal(vRight, RotationMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationMatrix);
		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);
	
		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);
		
	
		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = XMFLOAT4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vGravity = m_pOriginalGravity[i];
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}
		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}
	
	m_pContext->Unmap(m_pVBInstance, 0);
	
	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}

}

void CVIBuffer_Instance::Drop(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		pVertices[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;
		pVertices[i].vLifeTime.y += fTimeDelta;



		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::GrowOut(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] += fTimeDelta * 0.01f * m_pSpeeds[i];

		pVertices[i].vRight.x = m_pSize[i];
		pVertices[i].vUp.y = m_pSize[i];
		pVertices[i].vLook.z = m_pSize[i];

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vRight.x = m_pOriginalSize[i];
				pVertices[i].vUp.y = m_pOriginalSize[i];
				pVertices[i].vLook.z = m_pOriginalSize[i];
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::Spread_Size_Up(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] += fTimeDelta * 0.1f * m_pSpeeds[i];
		if (m_pSize[i] < 0.f)
			m_pSize[i] = 0.f;

		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc.vOffsetPos), 0.f);
		/*pVertices[i].vGravity += fTimeDelta;*/

		pVertices[i].vTranslation.y -= pVertices[i].vGravity * fTimeDelta;


		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		_matrix		RotationMatrix = XMMatrixRotationAxis(vRight, XMConvertToRadians(360) * fTimeDelta);
		vRight = XMVector3TransformNormal(vRight, RotationMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationMatrix);
		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);


		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vGravity = m_pOriginalGravity[i];
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}

		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}

	}


	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}



}

void CVIBuffer_Instance::Spread_Non_Rotation(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;

		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc.vOffsetPos), 0.f);
		/*pVertices[i].vGravity += fTimeDelta;*/

		pVertices[i].vTranslation.y -= pVertices[i].vGravity * fTimeDelta;


		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);


		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vGravity = m_pOriginalGravity[i];
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}
		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::CreateSwirlEffect(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// Calculate the current angle
		float angle = atan2(pVertices[i].vTranslation.z - m_InstanceDesc.vOffsetPos.z,
			pVertices[i].vTranslation.x - m_InstanceDesc.vOffsetPos.x);

		// Increase the angle over time to create a swirl effect
		angle += m_pSpeeds[i] * fTimeDelta;

		// Use vRange.z as the base radius and scale it with the y position
		float fBaseRadius = m_InstanceDesc.vRange.z;
		float currentRadius = fBaseRadius + pVertices[i].vTranslation.y * 0.2f; // Adjust the factor as needed

		// Calculate the new position
		float x = m_InstanceDesc.vOffsetPos.x + currentRadius * cos(angle);
		float z = m_InstanceDesc.vOffsetPos.z + currentRadius * sin(angle);

		// Update the position
		pVertices[i].vTranslation.x = x;
		pVertices[i].vTranslation.z = z;

		// Update the height based on gravity
		pVertices[i].vTranslation.y -= pVertices[i].vGravity * fTimeDelta;

		// Update direction vectors (Right, Up, Look)
		XMVECTOR vDir = XMVectorSet(pVertices[i].vTranslation.x - m_InstanceDesc.vOffsetPos.x,
			pVertices[i].vTranslation.y - m_InstanceDesc.vOffsetPos.y,
			pVertices[i].vTranslation.z - m_InstanceDesc.vOffsetPos.z, 0.0f);
		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);

		// Update lifetime
		pVertices[i].vLifeTime.y += fTimeDelta;
		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vGravity = m_pOriginalGravity[i];
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::Spread_Speed_Down(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;

		m_pSpeeds[i] -= fTimeDelta * pVertices[i].vGravity;
		if (m_pSpeeds[i] < 0.f)
			m_pSpeeds[i] = 0.f;

		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc.vOffsetPos), 0.f);

		//pVertices[i].vTranslation.y -= pVertices[i].vGravity * fTimeDelta;


		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);


		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				//pVertices[i].vGravity = m_pOriginalGravity[i];
				m_pSize[i] = m_pOriginalSize[i];
				m_pSpeeds[i] = m_pOriginalSpeed[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}
		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::SlashEffect(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;

		m_pSize[i] += fTimeDelta * m_pSpeeds[i];

		_vector			vDir = XMVectorSet(0.f, 0.f, 0.f, 1.f) - XMLoadFloat4(&pVertices[i].vTranslation);


		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pOriginalSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pOriginalSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);


		//XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				//pVertices[i].vGravity = m_pOriginalGravity[i];
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}
		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}

}

void CVIBuffer_Instance::Spread_Speed_Down_SizeUp(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;

		m_pSize[i] += fTimeDelta *  pVertices[i].vGravity;
		if (m_pSize[i] < 0.f)
			m_pSize[i] = 0.f;


		m_pSpeeds[i] -= fTimeDelta * pVertices[i].vGravity;
		if (m_pSpeeds[i] < 0.f)
			m_pSpeeds[i] = 0.f;

		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc.vOffsetPos), 0.f);

		//pVertices[i].vTranslation.y -= pVertices[i].vGravity * fTimeDelta;


		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);


		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				m_pSize[i] = m_pOriginalSize[i];
				m_pSpeeds[i] = m_pOriginalSpeed[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}
		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}

}

void CVIBuffer_Instance::Gather(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;


		m_pSize[i] -= fTimeDelta * pVertices[i].vGravity;
		if (m_pSize[i] < 0.f)
			m_pSize[i] = 0.f;


		_vector			vDir = XMVectorSet(0.f, 0.f, 0.f, 1.f) - XMLoadFloat4(&pVertices[i].vTranslation);

		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);


		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}
		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}

}

void CVIBuffer_Instance::Extinction(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] -= fTimeDelta * pVertices[i].vGravity;

		pVertices[i].vRight.x = m_pSize[i];
		pVertices[i].vUp.y = m_pSize[i];
		pVertices[i].vLook.z = m_pSize[i];


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vRight.x = m_pOriginalSize[i];
				pVertices[i].vUp.y = m_pOriginalSize[i];
				pVertices[i].vLook.z = m_pOriginalSize[i];
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}

}

void CVIBuffer_Instance::GrowOutY(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] += fTimeDelta * 0.01f * m_pSpeeds[i];

		pVertices[i].vUp.y = m_pSize[i];


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vRight.x = m_pOriginalSize[i];
				pVertices[i].vUp.y = m_pOriginalSize[i];
				pVertices[i].vLook.z = m_pOriginalSize[i];
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::GrowOut_Speed_Down(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] += fTimeDelta * m_pSpeeds[i];
		m_pSpeeds[i] -= fTimeDelta * 0.1f;

		_vector			vDir = XMVectorSet(0.f, 0.f, 0.f, 1.f) - XMLoadFloat4(&pVertices[i].vTranslation);
		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);


		//pVertices[i].vRight.x = m_pSize[i];
		//pVertices[i].vUp.y = m_pSize[i];
		//pVertices[i].vLook.z = m_pSize[i];

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vRight.x = m_pOriginalSize[i];
				pVertices[i].vUp.y = m_pOriginalSize[i];
				pVertices[i].vLook.z = m_pOriginalSize[i];
				m_pSize[i] = m_pOriginalSize[i];
				m_pSpeeds[i] = m_pOriginalSpeed[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::GrowOut_Speed_Down_Texture(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] += fTimeDelta * m_pSpeeds[i];
		m_pSpeeds[i] -= fTimeDelta * 0.1f;


		pVertices[i].vRight.x = m_pSize[i];
		pVertices[i].vUp.y = m_pSize[i];
		pVertices[i].vLook.z = m_pSize[i];

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vRight.x = m_pOriginalSize[i];
				pVertices[i].vUp.y = m_pOriginalSize[i];
				pVertices[i].vLook.z = m_pOriginalSize[i];
				m_pSize[i] = m_pOriginalSize[i];
				m_pSpeeds[i] = m_pOriginalSpeed[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::Lenz_Flare(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] += fTimeDelta * m_pSpeeds[i];
		m_pSpeeds[i] -= fTimeDelta * 0.1f;

		_vector vRight, vUp, vLook;
		vRight = XMLoadFloat4(&pVertices[i].vRight);
		vUp = XMLoadFloat4(&pVertices[i].vUp);
		vLook = XMLoadFloat4(&pVertices[i].vLook);

		_vector    vQuternion = XMQuaternionRotationAxis(XMVector4Normalize(vLook), fTimeDelta * pVertices[i].vGravity);
		_matrix    QuternionMatrix = XMMatrixRotationQuaternion(vQuternion);

		vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
		vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
		vLook = XMVector3TransformNormal(vLook, QuternionMatrix);

		XMStoreFloat4(&pVertices[i].vRight, XMVector3Normalize(vRight) * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, XMVector3Normalize(vUp) * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vLook) * m_pSize[i]);

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vRight.x = m_pOriginalSize[i];
				pVertices[i].vUp.y = m_pOriginalSize[i];
				pVertices[i].vLook.z = m_pOriginalSize[i];
				m_pSize[i] = m_pOriginalSize[i];
				m_pSpeeds[i] = m_pOriginalSpeed[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}

}

void CVIBuffer_Instance::Spiral_Extinction(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// Calculate the current angle
		float angle = atan2(pVertices[i].vTranslation.z - m_InstanceDesc.vOffsetPos.z,
			pVertices[i].vTranslation.x - m_InstanceDesc.vOffsetPos.x);

		// Increase the angle over time to create a swirl effect
		angle += m_pSpeeds[i] * fTimeDelta;

		// Calculate the distance from the center
		XMVECTOR vDist = XMVectorSet(
			pVertices[i].vTranslation.x - m_InstanceDesc.vOffsetPos.x,
			pVertices[i].vTranslation.y - m_InstanceDesc.vOffsetPos.y,
			pVertices[i].vTranslation.z - m_InstanceDesc.vOffsetPos.z,
			0.0f
		);
		float distance = XMVectorGetX(XMVector3Length(vDist));

		// Gradually decrease the radius using m_pSpeeds
		float radiusDecreaseFactor = m_pSpeeds[i] * 0.01f; // Adjust this value to control the spiral tightness
		float currentRadius = distance * (1.0f - radiusDecreaseFactor * pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x);

		// Calculate the new position
		float x = m_InstanceDesc.vOffsetPos.x + currentRadius * cos(angle);
		float z = m_InstanceDesc.vOffsetPos.z + currentRadius * sin(angle);

		// Update the position
		pVertices[i].vTranslation.x = x;
		pVertices[i].vTranslation.z = z;

		// Update the height based on gravity
		pVertices[i].vTranslation.y -= pVertices[i].vGravity * fTimeDelta;

		// Update direction vectors (Right, Up, Look)
		XMVECTOR vDir = XMVectorSet(pVertices[i].vTranslation.x - m_InstanceDesc.vOffsetPos.x,
			pVertices[i].vTranslation.y - m_InstanceDesc.vOffsetPos.y,
			pVertices[i].vTranslation.z - m_InstanceDesc.vOffsetPos.z, 0.0f);
		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);

		// Update lifetime and size
		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] -= fTimeDelta * pVertices[i].vGravity * 0.05f; // Gradually decrease size

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (m_InstanceDesc.isLoop)
			{
				// Reset position with some randomness
				float randomAngle = (float)rand() / RAND_MAX * XM_2PI;
				float randomRadius = ((float)rand() / RAND_MAX) * m_InstanceDesc.vRange.z;
				pVertices[i].vTranslation = _float4(
					m_InstanceDesc.vOffsetPos.x + cos(randomAngle) * randomRadius,
					m_InstanceDesc.vOffsetPos.y + ((float)rand() / RAND_MAX - 0.5f) * m_InstanceDesc.vRange.y,
					m_InstanceDesc.vOffsetPos.z + sin(randomAngle) * randomRadius,
					1.f
				);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vGravity = m_pOriginalGravity[i];
				m_pSize[i] = m_pOriginalSize[i];
				m_pSpeeds[i] = m_pOriginalSpeed[i] + ((float)rand() / RAND_MAX - 0.5f) * 0.5f; // Add some randomness to speed
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::Spiral_Expansion(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};
	bool allInstancesExpanded = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// Calculate the current angle
		float angle = atan2(pVertices[i].vTranslation.z - m_InstanceDesc.vOffsetPos.z,
			pVertices[i].vTranslation.x - m_InstanceDesc.vOffsetPos.x);

		// Increase the angle over time to create a swirl effect
		angle += m_pSpeeds[i] * fTimeDelta;

		// Calculate the distance from the center
		XMVECTOR vDist = XMVectorSet(
			pVertices[i].vTranslation.x - m_InstanceDesc.vOffsetPos.x,
			pVertices[i].vTranslation.y - m_InstanceDesc.vOffsetPos.y,
			pVertices[i].vTranslation.z - m_InstanceDesc.vOffsetPos.z,
			0.0f
		);
		float distance = XMVectorGetX(XMVector3Length(vDist));

		// Gradually increase the radius using m_pSpeeds
		float radiusIncreaseFactor = m_pSpeeds[i] * 0.05f; // Adjust this value to control the expansion speed
		float currentRadius = distance + radiusIncreaseFactor * pVertices[i].vLifeTime.y;

		// Calculate the new position
		float x = m_InstanceDesc.vOffsetPos.x + currentRadius * cos(angle);
		float z = m_InstanceDesc.vOffsetPos.z + currentRadius * sin(angle);

		// Update the position
		pVertices[i].vTranslation.x = x;
		pVertices[i].vTranslation.z = z;

		// Update the height based on an upward force (opposite of gravity)
		pVertices[i].vTranslation.y += pVertices[i].vGravity * fTimeDelta * 0.5f;

		// Update direction vectors (Right, Up, Look)
		XMVECTOR vDir = XMVectorSet(pVertices[i].vTranslation.x - m_InstanceDesc.vOffsetPos.x,
			pVertices[i].vTranslation.y - m_InstanceDesc.vOffsetPos.y,
			pVertices[i].vTranslation.z - m_InstanceDesc.vOffsetPos.z, 0.0f);
		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);

		// Update lifetime and size
		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] += fTimeDelta * pVertices[i].vGravity * 0.02f; // Gradually increase size

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (m_InstanceDesc.isLoop)
			{
				// Reset position to center
				pVertices[i].vTranslation = _float4(m_InstanceDesc.vOffsetPos.x, m_InstanceDesc.vOffsetPos.y, m_InstanceDesc.vOffsetPos.z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vGravity = m_pOriginalGravity[i];
				m_pSize[i] = m_pOriginalSize[i] * 0.5f; // Start with smaller size
				m_pSpeeds[i] = m_pOriginalSpeed[i] + ((float)rand() / RAND_MAX - 0.5f) * 0.5f; // Add some randomness to speed
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesExpanded = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesExpanded)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}


void CVIBuffer_Instance::Leaf_Fall(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		pVertices[i].vLifeTime.y += fTimeDelta;

		_float fRatio = pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x;
		_vector vRight, vUp, vLook, vPos;
		vRight = XMLoadFloat4(&pVertices[i].vRight);
		vUp = XMLoadFloat4(&pVertices[i].vUp);
		vLook = XMLoadFloat4(&pVertices[i].vLook);
		vPos = XMLoadFloat4(&pVertices[i].vTranslation);

		if (fRatio <= 0.2f)
		{
			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vRight), XMConvertToRadians(m_pSpeeds[i]));
			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
		}
		else if (fRatio > 0.2f && fRatio <= 0.4f)
		{
			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vUp), XMConvertToRadians(m_pSpeeds[i]));
			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
		}
		else if (fRatio > 0.4f && fRatio <= 0.6f)
		{
			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vRight), XMConvertToRadians(m_pSpeeds[i]));
			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
		}
		else if (fRatio > 0.6f && fRatio <= 0.8f)
		{
			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vUp), XMConvertToRadians(m_pSpeeds[i]));
			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
		}
		else
		{
			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vRight), XMConvertToRadians(m_pSpeeds[i]));
			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
		}

		_vector vDir = XMVector4Normalize(vLook);
		vPos += vDir * m_pSpeeds[i] * fTimeDelta;




		XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(vRight) * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(vUp)* m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(vLook) * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vTranslation, vPos);

		pVertices[i].vTranslation.y -= pVertices[i].vGravity * fTimeDelta;

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vRight.x = m_pOriginalSize[i];
				pVertices[i].vUp.y = m_pOriginalSize[i];
				pVertices[i].vLook.z = m_pOriginalSize[i];
				m_pSize[i] = m_pOriginalSize[i];
				m_pSpeeds[i] = m_pOriginalSpeed[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}


}

void CVIBuffer_Instance::Blow(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;

		//파티클 움직임 로직
		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc.vOffsetPos), 0.f);

		_vector vRight = XMLoadFloat4(&pVertices[i].vRight);
		_vector vUp = XMLoadFloat4(&pVertices[i].vUp);
		_vector vLook = XMLoadFloat4(&pVertices[i].vLook);
		_vector vPos = XMLoadFloat4(&pVertices[i].vTranslation);

		_vector Quat1 = XMQuaternionRotationAxis(XMVector4Normalize(vRight), XMConvertToRadians(pVertices[i].vGravity));
		_vector Quat2 = XMQuaternionRotationAxis(XMVector4Normalize(vUp), XMConvertToRadians(pVertices[i].vGravity));
		_vector Axis = XMQuaternionMultiply(Quat1, Quat2);

		_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
		vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
		vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
		vLook = XMVector3TransformNormal(vLook, QuternionMatrix);

		vPos += vDir * m_pSpeeds[i] * fTimeDelta;

		XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(vRight) * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(vUp) * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(vLook) * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vTranslation, vPos);


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vRight.x = m_pOriginalSize[i];
				pVertices[i].vUp.y = m_pOriginalSize[i];
				pVertices[i].vLook.z = m_pOriginalSize[i];
				m_pSize[i] = m_pOriginalSize[i];
				m_pSpeeds[i] = m_pOriginalSpeed[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}
		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}
	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}

}

void CVIBuffer_Instance::Up_To_Stop(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;
		pVertices[i].vGravity -= fTimeDelta; //진짜 라이프타임으로 쓰는거임

		pVertices[i].vTranslation.y += m_pSpeeds[i] * fTimeDelta;

		_vector vDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
		_vector vUp = XMVectorSet(0.f, 0.f, 1.f, 0.f);

		XMStoreFloat4(&pVertices[i].vLook, vDir * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);

		if (pVertices[i].vGravity < 0.f)
		{
			m_pSpeeds[i] += pVertices[i].vGravity * 0.05f;
			if (m_pSpeeds[i] < 0.f)
			{
				m_pSpeeds[i] = 0.f;
				allInstancesDead = true;
			}
			else
				allInstancesDead = false;
		}
		else
			allInstancesDead = false;

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x && !allInstancesDead)
		{
			pVertices[i].vLifeTime.y = 0.f;
			pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
		}
	}
	m_pContext->Unmap(m_pVBInstance, 0);

	m_bInstanceDead = allInstancesDead;
}

void CVIBuffer_Instance::Only_Up(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;
		pVertices[i].vGravity -= fTimeDelta;

		pVertices[i].vTranslation.y += m_pSpeeds[i] * fTimeDelta;

		_vector vDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
		_vector vUp = XMVectorSet(0.f, 0.f, 1.f, 0.f);

		XMStoreFloat4(&pVertices[i].vLook, vDir * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);

		if (pVertices[i].vGravity < 0.f)
		{
			pVertices[i].vGravity = m_pOriginalGravity[i];
			pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
		}

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
			pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
		else
			allInstancesDead = false;

	}
	m_pContext->Unmap(m_pVBInstance, 0);

	m_bInstanceDead = allInstancesDead;
}







void CVIBuffer_Instance::Initial_RotateY()
{
	D3D11_MAPPED_SUBRESOURCE      SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_matrix      RotationMatrix = XMMatrixRotationAxis(XMLoadFloat4(&pVertices[i].vUp),
			XMConvertToRadians(RandomFloat(0.f, 360.f)));

		_vector Right = XMVector4Normalize(XMLoadFloat4(&pVertices[i].vRight));
		_vector Up = XMVector4Normalize(XMLoadFloat4(&pVertices[i].vUp));
		_vector Look = XMVector4Normalize(XMLoadFloat4(&pVertices[i].vLook));

		Right = XMVector3TransformNormal(Right, RotationMatrix);
		Up = XMVector3TransformNormal(Up, RotationMatrix);
		Look = XMVector3TransformNormal(Look, RotationMatrix);

		XMStoreFloat4(&pVertices[i].vRight, Right * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, Up * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, Look * m_pSize[i]);


	}
	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::Initial_RandomOffset(CVIBuffer_Terrain* pTerrain)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> dis(-1.0f, 1.0f);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// 현재 위치 저장
		_float3 currentPos = _float3(pVertices[i].vTranslation.x, pVertices[i].vTranslation.y, pVertices[i].vTranslation.z);

		// 랜덤 오프셋 생성 (X와 Z 축에 대해서만)
		float offsetX = dis(gen) * 1.f; // 오프셋 범위를 줄임
		float offsetZ = dis(gen) * 1.f;

		// Perlin Noise를 사용하여 더 자연스러운 오프셋 생성
		float noiseX = PerlinNoise(currentPos.x * 0.1f, currentPos.y * 0.1f, currentPos.z * 0.1f);
		float noiseZ = PerlinNoise(currentPos.z * 0.1f, currentPos.x * 0.1f, currentPos.y * 0.1f);

		// Perlin Noise 값을 -1에서 1 사이의 범위로 변환
		noiseX = (noiseX * 2.0f - 1.0f) * 1.f;
		noiseZ = (noiseZ * 2.0f - 1.0f) * 1.f;

		// 최종 오프셋 계산 (랜덤 오프셋과 노이즈 기반 오프셋의 조합)
		float finalOffsetX = (offsetX + noiseX) * 0.5f;
		float finalOffsetZ = (offsetZ + noiseZ) * 0.5f;

		// 오프셋 적용
		currentPos.x += finalOffsetX;
		currentPos.z += finalOffsetZ;

		// 새로운 위치에서의 높이 계산
		float newHeight = pTerrain->Compute_Height(currentPos);

		// 새 위치 적용
		pVertices[i].vTranslation = _float4(currentPos.x, newHeight, currentPos.z, 1.f);

		// 노멀 계산 및 회전 적용 (Setup_Onterrain과 유사한 방식)
		_float3 vNormal = pTerrain->Compute_Normal(currentPos);
		_vector vUp = { 0, 1, 0, 0.f };
		_vector vTerrainNormal = XMLoadFloat3(&vNormal);

		if (!XMVector3NearEqual(vUp, vTerrainNormal, XMVectorReplicate(0.0001f)))
		{
			_vector vRotationAxis = XMVector3Normalize(XMVector3Cross(vUp, vTerrainNormal));
			float fRotationAngle = XMScalarACos(XMVector3Dot(vUp, vTerrainNormal).m128_f32[0]);
			_matrix rotationMatrix = XMMatrixRotationAxis(vRotationAxis, fRotationAngle);
			_matrix currentRotation = XMMatrixIdentity();
			currentRotation.r[0] = XMLoadFloat4(&pVertices[i].vRight);
			currentRotation.r[1] = XMLoadFloat4(&pVertices[i].vUp);
			currentRotation.r[2] = XMLoadFloat4(&pVertices[i].vLook);
			_matrix newRotation = XMMatrixMultiply(currentRotation, rotationMatrix);
			XMStoreFloat4(&pVertices[i].vRight, newRotation.r[0]);
			XMStoreFloat4(&pVertices[i].vUp, newRotation.r[1]);
			XMStoreFloat4(&pVertices[i].vLook, newRotation.r[2]);
		}

		// 크기 변화 추가 (선택적)
		float sizeVariation = PerlinNoise(currentPos.x * 0.2f, currentPos.y * 0.2f, currentPos.z * 0.2f);
		float newSize = m_pOriginalSize[i] * (0.8f + sizeVariation * 0.4f);  // 80% ~ 120% 변화

		// 새로운 크기 적용
		_vector Right = XMLoadFloat4(&pVertices[i].vRight);
		_vector Up = XMLoadFloat4(&pVertices[i].vUp);
		_vector Look = XMLoadFloat4(&pVertices[i].vLook);
		XMStoreFloat4(&pVertices[i].vRight, XMVector3Normalize(Right) * newSize);
		XMStoreFloat4(&pVertices[i].vUp, XMVector3Normalize(Up) * newSize);
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(Look) * newSize);
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::Initial_InsideCircle(float radius, _float3 pivot)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	XMVECTOR pivotVector = XMLoadFloat3(&pivot);
	XMVectorSetW(pivotVector, 1.f);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// 중심에 더 많은 인스턴스가 생성되도록 확률 분포 조정
		float u = RandomFloat(0.f, 1.f);
		float v = RandomFloat(0.f, 1.f);
		float randomRadius = radius * pow(u, 2); // 제곱을 사용하여 중심 쪽으로 치우치게 함
		float randomAngle = v * XM_2PI;

		XMVECTOR newPosition = XMVectorSet(
			pivot.x + randomRadius * cos(randomAngle),
			pivot.y,
			pivot.z + randomRadius * sin(randomAngle),
			1.0f
		);

		// 새 위치 저장
		XMStoreFloat4(&pVertices[i].vTranslation, newPosition);

		// 회전 적용 (y축 기준 랜덤 회전)
		XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(RandomFloat(0.f, 360.f)));
		XMVECTOR right = XMLoadFloat4(&pVertices[i].vRight);
		XMVECTOR up = XMLoadFloat4(&pVertices[i].vUp);
		XMVECTOR look = XMLoadFloat4(&pVertices[i].vLook);

		right = XMVector3TransformNormal(right, rotationMatrix);
		up = XMVector3TransformNormal(up, rotationMatrix);
		look = XMVector3TransformNormal(look, rotationMatrix);

		XMStoreFloat4(&pVertices[i].vRight, XMVector3Normalize(right) * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, XMVector3Normalize(up) * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(look) * m_pSize[i]);
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::Culling_Instance(const _float3& cameraPosition, _float maxRenderDistance)
{
	

	// 1. 버퍼 유효성 확인
	if (!m_pVBInstance)
	{
		// 로그 기록 또는 에러 처리
		return;
	}

	//D3D11_MAPPED_SUBRESOURCE mappedResource;
	//HRESULT hr = m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
	////if (FAILED(hr) || !mappedResource.pData)
	////{
	////	// 매핑 실패 처리
	////	// 로그 기록 또는 에러 처리
	////	return;
	////}

	//VTXMATRIX* pInstanceData = static_cast<VTXMATRIX*>(mappedResource.pData);

	



	//// 2. 최적화: 거리 계산을 위한 상수 준비
	//const XMVECTOR camPos = XMLoadFloat3(&cameraPosition);
	//const float maxDistanceSquared = maxRenderDistance * maxRenderDistance;

	//// 3. 가시성 검사 및 데이터 재정렬
	//unsigned int visibleCount = 0;



	//for (unsigned int i = 0; i < m_iNumInstance; ++i)
	//{
	//	m_pGameInstance->AddWork([pInstanceData, i, maxDistanceSquared, camPos, &visibleCount]()
	//		{
	//			XMVECTOR instancePos = XMLoadFloat4(&pInstanceData[i].vTranslation);
	//			float distanceSquared = XMVectorGetX(XMVector3LengthSq(XMVectorSubtract(instancePos, camPos)));

	//			if (distanceSquared <= maxDistanceSquared)
	//			{
	//				if (i != visibleCount)
	//				{
	//					// 자리 바꾸기 (swap)
	//					VTXMATRIX temp = pInstanceData[visibleCount];
	//					pInstanceData[visibleCount] = pInstanceData[i];
	//					pInstanceData[i] = temp;
	//				}
	//				++visibleCount; 
	//			}
	//		}
	//	);

	//	
	//}

	//m_pContext->Unmap(m_pVBInstance, 0);

	// 4. 가시 인스턴스 수 업데이트 (원자적 업데이트 없이)
	//m_iVisibleInstances = visibleCount;



	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
	//if (FAILED(hr) || !mappedResource.pData)
	//{
	//	// 매핑 실패 처리
	//	// 로그 기록 또는 에러 처리
	//	return;
	//}

	VTXMATRIX* pInstanceData = static_cast<VTXMATRIX*>(mappedResource.pData);


	//cudaMalloc(&d_pInstanceData, sizeof(VTXMATRIX) * m_iNumInstance);		//메모리 할당
	//cudaMalloc(&d_visibleCount, sizeof(int));								//메모리 할당
	//
	cudaMemcpy(d_pInstanceData, pInstanceData, sizeof(VTXMATRIX) * m_iNumInstance, cudaMemcpyHostToDevice);		//디바이스에 복사
	cudaMemset(d_visibleCount, 0, sizeof(int));																//초기화

	float3 cudaCameraPos = make_float3(cameraPosition.x, cameraPosition.y, cameraPosition.z);
	CCuda::LaunchKernel_CullingInstance(d_pInstanceData, m_iNumInstance, cudaCameraPos, maxRenderDistance, d_visibleCount);		//커널 실행



	cudaMemcpy(pInstanceData, d_pInstanceData, sizeof(VTXMATRIX) * m_iNumInstance, cudaMemcpyDeviceToHost);		//호스트로 복사
	cudaMemcpy(&m_iVisibleInstances, d_visibleCount, sizeof(int), cudaMemcpyDeviceToHost);						//호스트로 복사



	m_pContext->Unmap(m_pVBInstance, 0);


	//cudaFree(d_pInstanceData);		//메모리 해제
	//cudaFree(d_visibleCount);		//메모리 해제


}

vector<_float4x4*> CVIBuffer_Instance::Get_VtxMatrices()
{
	vector<_float4x4*> worldMats;
	D3D11_MAPPED_SUBRESOURCE SubResource{};
	if (FAILED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource)))
	{
		return worldMats; // 맵핑 실패시 빈 벡터 반환
	}
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float4x4* mat = new _float4x4();  // 동적 할당
		// 정규화된 방향 벡터 계산
		_vector Right = XMVector4Normalize(XMLoadFloat4(&pVertices[i].vRight));
		_vector Up = XMVector4Normalize(XMLoadFloat4(&pVertices[i].vUp));
		_vector Look = XMVector4Normalize(XMLoadFloat4(&pVertices[i].vLook));
		// 행렬 구성
		XMStoreFloat4x4(mat, XMMatrixSet(
			Right.m128_f32[0], Right.m128_f32[1], Right.m128_f32[2], 0.0f,
			Up.m128_f32[0], Up.m128_f32[1], Up.m128_f32[2], 0.0f,
			Look.m128_f32[0], Look.m128_f32[1], Look.m128_f32[2], 0.0f,
			pVertices[i].vTranslation.x, pVertices[i].vTranslation.y, pVertices[i].vTranslation.z, 1.0f
		));
		worldMats.push_back(mat);
	}
	m_pContext->Unmap(m_pVBInstance, 0);
	return worldMats;
}

HRESULT CVIBuffer_Instance::Render_Culling()
{
	//// 컬링 파라미터 업데이트
	//CullingParams* pCullingParams = (CullingParams*)m_pCullingCSCom->GetConstantBuffer();
	//pCullingParams->cameraPosition = m_pGameInstance->Get_CamPosition_float3();
	//pCullingParams->maxRenderDistance = 1000.0f; // 적절한 값으로 설정
	//pCullingParams->numInstances = m_iNumInstance;

	//// 컴퓨트 셰이더 실행
	//UINT* visibilityFlags = (UINT*)m_pCullingCSCom->Compute(m_iNumInstance / 256 + 1, 1, 1);
	//if (visibilityFlags == nullptr)
	//	return E_FAIL;

	//// 가시적인 인스턴스 수 계산
	//m_iVisibleInstances = 0;
	//for (UINT i = 0; i < m_iNumInstance; ++i)
	//{
	//	if (visibilityFlags[i] == 1)
	//		++m_iVisibleInstances;
	//}

	//// 가시적인 인스턴스만 렌더링
	


	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iVisibleInstances, 0, 0, 0);


	return S_OK;
}
void CVIBuffer_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeeds);
		Safe_Delete_Array(m_pOriginalSpeed);
		Safe_Delete_Array(m_pOriginalPositions);
		Safe_Delete_Array(m_pOriginalGravity);
		Safe_Delete_Array(m_pSize);
		Safe_Delete_Array(m_pOriginalSize);
	}

	Safe_Release(m_pVBInstance);
}
