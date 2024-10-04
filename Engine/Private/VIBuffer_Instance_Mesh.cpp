#include "VIBuffer_Instance_Mesh.h"
#include "Mesh.h"
CInstance_Mesh::CInstance_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CVIBuffer_Instance(pDevice, pContext)
{
}

CInstance_Mesh::CInstance_Mesh(const CInstance_Mesh& rhs)
	:CVIBuffer_Instance(rhs)
{
}

HRESULT CInstance_Mesh::Initialize_ProtoType(CMesh* m_Meshes, const INSTANCE_DESC& InstanceDesc)
{
	m_InstanceDesc = InstanceDesc;

	if (FAILED(__super::Initialize_Prototype(InstanceDesc)))
		return E_FAIL;

	m_iMaterialIndex = m_Meshes->Get_MaterialIndex();

	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iNumVertexBuffers = 2;
	m_iNumVertices = m_Meshes->Get_NumVertex();
	m_iIndexStride = 4;
	m_iIndexCountPerInstance = m_Meshes->Get_NumIndices()*3;
	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;

	m_iInstanceStride = sizeof(VTXMATRIX);
#pragma region VerTex

	m_iVertexStride = sizeof(VTXMESH);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);
	
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i], &m_Meshes->Get_Vertices()[i], sizeof(VTXMESH));
	}

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


	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);
	_uint      iNumIndices = { 0 };

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_uint iFaceCnt = m_Meshes->Get_NumIndices() / 3;
		_uint      iNumFacesIndices = { 0 };
		for (size_t j = 0; j < iFaceCnt; j++)
		{
			pIndices[iNumIndices++] = m_Meshes->Get_Mesh_Indices()[iNumFacesIndices++];
			pIndices[iNumIndices++] = m_Meshes->Get_Mesh_Indices()[iNumFacesIndices++];
			pIndices[iNumIndices++] = m_Meshes->Get_Mesh_Indices()[iNumFacesIndices++];
		}

	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);


#pragma endregion INDEX_BUFFER

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



HRESULT CInstance_Mesh::Initialize_ProtoType_ForMapElements(CMesh* m_Meshes, const INSTANCE_MAP_DESC& InstanceDesc)
{
	m_InstanceMapDesc = InstanceDesc;

	m_iNumInstance = InstanceDesc.iNumInstance;
	m_pDevice->CreateBuffer(&m_InstanceBufferDesc, nullptr, &m_pVBInstance);
	m_RandomNumber = mt19937_64(m_RandomDevice());

	m_iMaterialIndex = m_Meshes->Get_MaterialIndex();

	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iNumVertexBuffers = 2;
	m_iNumVertices = m_Meshes->Get_NumVertex();
	m_iIndexStride = 4;
	m_iIndexCountPerInstance = /*m_Meshes->Get_NumVertex()*/m_Meshes->Get_NumIndices()*3;
	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;

	m_iInstanceStride = sizeof(VTXMATRIX);
#pragma region VerTex

	m_iVertexStride = sizeof(VTXMESH);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);
	
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i], &m_Meshes->Get_Vertices()[i], sizeof(VTXMESH));
	}

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


	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);
	_uint      iNumIndices = { 0 };

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_uint iFaceCnt = m_Meshes->Get_NumIndices() / 3;
		_uint      iNumFacesIndices = { 0 };
		for (size_t j = 0; j < iFaceCnt; j++)
		{
			pIndices[iNumIndices++] = m_Meshes->Get_Mesh_Indices()[iNumFacesIndices++];
			pIndices[iNumIndices++] = m_Meshes->Get_Mesh_Indices()[iNumFacesIndices++];
			pIndices[iNumIndices++] = m_Meshes->Get_Mesh_Indices()[iNumFacesIndices++];
		}

	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);


#pragma endregion INDEX_BUFFER

#pragma region INSTANCE_BUFFER
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	VTXMATRIX* pInstanceVertices = new VTXMATRIX[m_iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX) * m_iNumInstance);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		if (i < InstanceDesc.WorldMats.size())
		{
			// WorldMats에서 월드 행렬을 가져와 VTXMATRIX 구조체에 설정
			XMMATRIX worldMatrix = XMLoadFloat4x4(InstanceDesc.WorldMats[i]);
		
			// 월드 행렬의 각 행을 VTXMATRIX의 각 벡터에 복사
			XMStoreFloat4(&pInstanceVertices[i].vRight, (worldMatrix.r[0]));
			XMStoreFloat4(&pInstanceVertices[i].vUp, (worldMatrix.r[1]));
			XMStoreFloat4(&pInstanceVertices[i].vLook, (worldMatrix.r[2]));
			XMStoreFloat4(&pInstanceVertices[i].vTranslation, (worldMatrix.r[3]));
		}
		else
		{
			// WorldMats의 크기가 m_iNumInstance보다 작을 경우, 기본값으로 설정
			pInstanceVertices[i].vRight = _float4(1.0f, 0.0f, 0.0f, 0.0f);
			pInstanceVertices[i].vUp = _float4(0.0f, 1.0f, 0.0f, 0.0f);
			pInstanceVertices[i].vLook = _float4(0.0f, 0.0f, 1.0f, 0.0f);
			pInstanceVertices[i].vTranslation = _float4(0.0f, 0.0f, 0.0f, 1.0f);
		}
	
	}

	m_InitialData.pSysMem = pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InitialData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVertices);
#pragma endregion

	return S_OK;
}

HRESULT CInstance_Mesh::Initialize(void* pArg)
{
	
	return S_OK;
}



CInstance_Mesh* CInstance_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CMesh* m_Meshes, const INSTANCE_DESC& InstanceDesc)
{
	CInstance_Mesh* pInstance = new CInstance_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_ProtoType(m_Meshes,  InstanceDesc)))
	{
		MSG_BOX("Failed To Created : InstanceMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CInstance_Mesh* CInstance_Mesh::Create_ForMapElements(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CMesh* m_Meshes, const INSTANCE_MAP_DESC& InstanceDesc)
{
	CInstance_Mesh* pInstance = new CInstance_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_ProtoType_ForMapElements(m_Meshes, InstanceDesc)))
	{
		MSG_BOX("Failed To Created : InstanceMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CInstance_Mesh::Clone(void* pArg)
{
	CInstance_Mesh* pInstance = new CInstance_Mesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CInstance_Model");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInstance_Mesh::Free()
{

	__super::Free();
}
