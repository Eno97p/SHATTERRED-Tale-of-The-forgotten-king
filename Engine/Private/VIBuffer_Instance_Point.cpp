#include "..\Public\VIBuffer_Instance_Point.h"
#include "VIBuffer_Terrain.h"
#include "ComputeShader_Buffer.h"
CVIBuffer_Instance_Point::CVIBuffer_Instance_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_Instance_Point::CVIBuffer_Instance_Point(const CVIBuffer_Instance_Point& rhs)
	: CVIBuffer_Instance{ rhs }
{
}

CVIBuffer_Instance_Point::CVIBuffer_Instance_Point(const CVIBuffer_Instance_Point& rhs, _bool For_Copy)
	: CVIBuffer_Instance{ rhs}	
{
	m_pContext->CopyResource(m_pVBInstance, rhs.m_pVBInstance);
}

HRESULT CVIBuffer_Instance_Point::Initialize_Prototype()
{



	return S_OK;
}

HRESULT CVIBuffer_Instance_Point::Initialize(void* pArg)
{
	INSTANCE_DESC InstanceDesc = *((INSTANCE_DESC*)pArg);

	if (FAILED(__super::Initialize_Prototype(InstanceDesc)))
		return E_FAIL;

	m_InstanceDesc = InstanceDesc;

	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_iIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iNumVertexBuffers = 2;
	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;

	m_iIndexStride = 2;
	m_iIndexCountPerInstance = 1;
	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;
	m_iInstanceStride = sizeof(VTXMATRIX);


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




#pragma region INSTANCE_BUFFER
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	VTXMATRIX* pInstanceVertices = new VTXMATRIX[m_iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX) * m_iNumInstance);

	m_pSpeeds = new _float[m_iNumInstance];
	ZeroMemory(m_pSpeeds, sizeof(_float) * m_iNumInstance);

	m_pOriginalSpeed = new _float[m_iNumInstance];
	ZeroMemory(m_pOriginalSpeed, sizeof(_float) * m_iNumInstance);

	m_pOriginalPositions = new _float3[m_iNumInstance];
	ZeroMemory(m_pOriginalPositions, sizeof(_float3) * m_iNumInstance);

	m_pOriginalGravity = new _float[m_iNumInstance];
	ZeroMemory(m_pOriginalGravity, sizeof(_float) * m_iNumInstance);

	m_pSize = new _float[m_iNumInstance];
	ZeroMemory(m_pSize, sizeof(_float) * m_iNumInstance);

	m_pOriginalSize = new _float[m_iNumInstance];
	ZeroMemory(m_pOriginalSize, sizeof(_float) * m_iNumInstance);


	uniform_real_distribution<float>	RangeX(InstanceDesc.vPivotPos.x - InstanceDesc.vRange.x * 0.5f, InstanceDesc.vPivotPos.x + InstanceDesc.vRange.x * 0.5f);
	uniform_real_distribution<float>	RangeY(InstanceDesc.vPivotPos.y - InstanceDesc.vRange.y * 0.5f, InstanceDesc.vPivotPos.y + InstanceDesc.vRange.y * 0.5f);
	uniform_real_distribution<float>	RangeZ(InstanceDesc.vPivotPos.z - InstanceDesc.vRange.z * 0.5f, InstanceDesc.vPivotPos.z + InstanceDesc.vRange.z * 0.5f);

	uniform_real_distribution<float>	Size(InstanceDesc.vSize.x, InstanceDesc.vSize.y);
	uniform_real_distribution<float>	Speed(InstanceDesc.vSpeed.x, InstanceDesc.vSpeed.y);

	uniform_real_distribution<float>	LifeTime(InstanceDesc.vLifeTime.x, InstanceDesc.vLifeTime.y);
	uniform_real_distribution<float>    Gravity(InstanceDesc.vGravity.x, InstanceDesc.vGravity.y);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		m_pOriginalSize[i] = Size(m_RandomNumber);
		m_pSize[i] = m_pOriginalSize[i];
		pInstanceVertices[i].vRight = _float4(m_pSize[i], 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, m_pSize[i], 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, m_pSize[i], 0.f);
		pInstanceVertices[i].vTranslation = _float4(RangeX(m_RandomNumber), RangeY(m_RandomNumber), RangeZ(m_RandomNumber), 1.f);
		m_pOriginalPositions[i] = _float3(pInstanceVertices[i].vTranslation.x, pInstanceVertices[i].vTranslation.y, pInstanceVertices[i].vTranslation.z);
		pInstanceVertices[i].vLifeTime.x = LifeTime(m_RandomNumber);
		m_pSpeeds[i] = Speed(m_RandomNumber);
		m_pOriginalSpeed[i] = m_pSpeeds[i];
		pInstanceVertices[i].vGravity = Gravity(m_RandomNumber);
		m_pOriginalGravity[i] = pInstanceVertices[i].vGravity;
	}
	m_InitialData.pSysMem = pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InitialData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVertices);
#pragma endregion



	
	return S_OK;
}

void CVIBuffer_Instance_Point::Setup_Onterrain(CVIBuffer_Terrain* pTerrain)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	const vector<_float3>& validPositions = pTerrain->GetValidGrassPositions();

	if (validPositions.empty())
	{
		// 유효한 위치가 없으면 모든 잔디를 숨김
		for (size_t i = 0; i < m_iNumInstance; i++)
		{
			pVertices[i].vTranslation = _float4(0.f, -1000.f, 0.f, 1.f);
		}
		m_pContext->Unmap(m_pVBInstance, 0);
		return;
	}

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(0, validPositions.size() - 1);
	uniform_real_distribution<float> offsetDis(-0.5f, 0.5f);


	VTXPOINT* pPointVertices = (VTXPOINT*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float3 vWorldPos = validPositions[dis(gen)];

		// 약간의 오프셋 추가하여 자연스러운 분포 생성
		vWorldPos.x += offsetDis(gen);
		vWorldPos.z += offsetDis(gen);

		// 높이 계산 (오프셋으로 인한 높이 변화 반영)
		float fTerrainHeight = pTerrain->Compute_Height(vWorldPos);

		// 노멀 계산
		_float3 vNormal = pTerrain->Compute_Normal(vWorldPos);

		// 위치 적용 (사이즈의 절반을 더함)
		pVertices[i].vTranslation = _float4(vWorldPos.x, fTerrainHeight, vWorldPos.z, 1.f);

		// 회전 적용
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

			// 크기 보존
			_vector scale = XMVectorSet(m_pSize[i], m_pSize[i], m_pSize[i], 0.0f);

			XMStoreFloat4(&pVertices[i].vRight, XMVector3Normalize(newRotation.r[0]) * XMVectorGetX(scale));
			XMStoreFloat4(&pVertices[i].vUp, XMVector3Normalize(newRotation.r[1]) * XMVectorGetY(scale));
			XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(newRotation.r[2]) * XMVectorGetZ(scale));
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Instance_Point::Ready_Instance_ForGrass(const CVIBuffer_Instance::INSTANCE_MAP_DESC& InstanceDesc)
{
	vector<_float4x4*> WorldMats = InstanceDesc.WorldMats;
	m_iNumInstance = WorldMats.size();

	// 기존 인스턴스 버퍼 해제
	Safe_Release(m_pVBInstance);

	// 새로운 인스턴스 버퍼 생성
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	VTXMATRIX* pInstanceVertices = new VTXMATRIX[m_iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX) * m_iNumInstance);

	for (size_t i = 0; i < m_iNumInstance; ++i)
	{
		// _float4x4*를 XMMATRIX로 변환
		XMMATRIX worldMatrix = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(WorldMats[i]));

		// XMMATRIX의 각 행을 XMVECTOR로 추출
		XMVECTOR row0 = worldMatrix.r[0];
		XMVECTOR row1 = worldMatrix.r[1];
		XMVECTOR row2 = worldMatrix.r[2];
		XMVECTOR row3 = worldMatrix.r[3];

		// XMVECTOR를 VTXMATRIX의 각 멤버에 저장
		XMStoreFloat4(&pInstanceVertices[i].vRight, row0);
		XMStoreFloat4(&pInstanceVertices[i].vUp, row1);
		XMStoreFloat4(&pInstanceVertices[i].vLook, row2);
		XMStoreFloat4(&pInstanceVertices[i].vTranslation, row3);
	}

	D3D11_SUBRESOURCE_DATA SubResourceData;
	ZeroMemory(&SubResourceData, sizeof SubResourceData);
	SubResourceData.pSysMem = pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &SubResourceData, &m_pVBInstance)))
	{
		Safe_Delete_Array(pInstanceVertices);
		return E_FAIL;
	}

	Safe_Delete_Array(pInstanceVertices);

	// 기존 데이터 해제
	Safe_Delete_Array(m_pSpeeds);
	Safe_Delete_Array(m_pOriginalSpeed);
	Safe_Delete_Array(m_pOriginalPositions);
	Safe_Delete_Array(m_pOriginalGravity);
	Safe_Delete_Array(m_pSize);
	Safe_Delete_Array(m_pOriginalSize);

	// 새로운 데이터 할당
	m_pSpeeds = new _float[m_iNumInstance];
	m_pOriginalSpeed = new _float[m_iNumInstance];
	m_pOriginalPositions = new _float3[m_iNumInstance];
	m_pOriginalGravity = new _float[m_iNumInstance];
	m_pSize = new _float[m_iNumInstance];
	m_pOriginalSize = new _float[m_iNumInstance];

	// 새로운 데이터 초기화
	for (size_t i = 0; i < m_iNumInstance; ++i)
	{
		m_pSpeeds[i] = 0.f;
		m_pOriginalSpeed[i] = 0.f;
		m_pOriginalPositions[i] = _float3((*WorldMats[i])._41, (*WorldMats[i])._42, (*WorldMats[i])._43);
		m_pOriginalGravity[i] = 0.f;
		m_pSize[i] = 1.f;
		m_pOriginalSize[i] = 1.f;
	}

	return S_OK;
}
CVIBuffer_Instance_Point* CVIBuffer_Instance_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Instance_Point* pInstance = new CVIBuffer_Instance_Point(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CVIBuffer_Instance_Point");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Instance_Point::Clone(void* pArg)
{
	CVIBuffer_Instance_Point* pInstance = new CVIBuffer_Instance_Point(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Instance_Point");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Instance_Point::Free()
{
	Safe_Delete_Array(m_pSpeeds);
	Safe_Delete_Array(m_pOriginalSpeed);
	Safe_Delete_Array(m_pOriginalPositions);
	Safe_Delete_Array(m_pOriginalGravity);
	Safe_Delete_Array(m_pSize);
	Safe_Delete_Array(m_pOriginalSize);
	
	__super::Free();

	//이민영 추가 240629
	Safe_Delete_Array(m_pSpeeds);
	Safe_Delete_Array(m_pOriginalSpeed);
	Safe_Delete_Array(m_pOriginalPositions);
	Safe_Delete_Array(m_pOriginalGravity);
	Safe_Delete_Array(m_pSize);
	Safe_Delete_Array(m_pOriginalSize);
}
