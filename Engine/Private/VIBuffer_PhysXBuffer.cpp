#include "..\Public\VIBuffer_PhysXBuffer.h"


#include"Model.h"
CVIBuffer_PhysXBuffer::CVIBuffer_PhysXBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{

}

CVIBuffer_PhysXBuffer::CVIBuffer_PhysXBuffer(const CVIBuffer_PhysXBuffer & rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CVIBuffer_PhysXBuffer::Initialize_Prototype(istream& is)
{

	if(FAILED(Load_Buffer(is)))
		return E_FAIL;





	return S_OK;
}

HRESULT CVIBuffer_PhysXBuffer::Initialize_Prototype(const PxRenderBuffer& rRenderBuffer)
{
	

	PxU32 PhysXBuffer_TriCount = rRenderBuffer.getNbTriangles();
	PxU32 PhysXBuffer_VerCount = rRenderBuffer.getNbTriangles() * 3;




	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iNumVertexBuffers = 1;

	m_iNumVertices = PhysXBuffer_VerCount;
	m_iIndexStride= 4;
	m_iNumIndices = PhysXBuffer_TriCount * 3;
	m_iNumPrimitive = PhysXBuffer_TriCount;

#pragma region VERTEX_BUFFER 

	m_iVertexStride= sizeof(VTXPHYSX);




	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPHYSX* 	pVertices = new VTXPHYSX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPHYSX) * m_iNumVertices);



	XMFLOAT3 defaultColor = XMFLOAT3(1.0f, 0.0f, 1.0f); // 보라색
	for(PxU32 i = 0; i < PhysXBuffer_TriCount; i++)
	{
		const PxDebugTriangle& triangle = rRenderBuffer.getTriangles()[i];
		pVertices[i * 3 + 0].vPosition = _float3(triangle.pos0.x, triangle.pos0.y, triangle.pos0.z);
		pVertices[i * 3 + 1].vPosition = _float3(triangle.pos1.x, triangle.pos1.y, triangle.pos1.z);
		pVertices[i * 3 + 2].vPosition = _float3(triangle.pos2.x, triangle.pos2.y, triangle.pos2.z);

		pVertices[i * 3 + 0].vColor = defaultColor;
		pVertices[i * 3 + 1].vColor = defaultColor;
		pVertices[i * 3 + 2].vColor = defaultColor;
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
//
#pragma endregion
//
#pragma region INDEX_BUFFER 
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_uint*		pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	for (UINT i = 0; i < PhysXBuffer_TriCount; ++i)
	{
		// 각 삼각형을 이루는 세 개의 정점에 대한 인덱스를 설정합니다.
		// 여기서는 각 정점이 순차적으로 생성되었으므로 i * 3, i * 3 + 1, i * 3 + 2로 설정합니다.
		pIndices[i * 3] = i * 3;
		pIndices[i * 3 + 1] = i * 3 + 1;
		pIndices[i * 3 + 2] = i * 3 + 2;
	}



	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion


	return S_OK;
}

HRESULT CVIBuffer_PhysXBuffer::Initialize_Prototype(const vector<_float3> vecVertices)
{

	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iNumVertexBuffers = 1;

	m_iNumVertices = vecVertices.size();
	m_iIndexStride = 4;
	m_iNumIndices = vecVertices.size();
	m_iNumPrimitive = vecVertices.size() / 3;
#pragma region VERTEX_BUFFER 

	m_iVertexStride = sizeof(VTXPHYSX);



	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPHYSX* pVertices = new VTXPHYSX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPHYSX) * m_iNumVertices);

	for(UINT i = 0; i < m_iNumVertices; i++)
	{
		pVertices[i].vPosition = vecVertices[i];
		pVertices[i].vColor = _float3(0.0f, 1.0f, 0.0f);
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



	for (UINT i = 0; i < m_iNumIndices; ++i)
	{
		pIndices[i] = i;
	}


	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion


	return S_OK;
}

HRESULT CVIBuffer_PhysXBuffer::Initialize_Prototype(const vector<_float3>& vecVertices, const std::vector<_uint>& vecIndices)
{
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iNumVertexBuffers = 1;

	m_iNumVertices = static_cast<_uint>(vecVertices.size());
	m_iIndexStride = 4; // 32비트 인덱스
	m_iNumIndices = static_cast<_uint>(vecIndices.size());
	m_iNumPrimitive = m_iNumIndices / 3;

#pragma region VERTEX_BUFFER 

	m_iVertexStride = sizeof(VTXPHYSX);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPHYSX* pVertices = new VTXPHYSX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPHYSX) * m_iNumVertices);

	for (UINT i = 0; i < m_iNumVertices; i++)
	{
		pVertices[i].vPosition = vecVertices[i];
		pVertices[i].vColor = _float3(0.0f, 1.0f, 0.0f);
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

	for (UINT i = 0; i < m_iNumIndices; ++i)
	{
		pIndices[i] = vecIndices[i];
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;



	
}

HRESULT CVIBuffer_PhysXBuffer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_PhysXBuffer::Load_Buffer(istream& is)
{	
		
			char szName[MAX_PATH] = "";
			is.read(szName, sizeof(_char) * MAX_PATH);
		
		
			is.read(reinterpret_cast<char*>(&m_iNumPrimitive), sizeof(_uint));
		
			_uint iTempMaterialCount = 0;
			is.read(reinterpret_cast<char*>(&iTempMaterialCount), sizeof(_uint));
		
		
			is.read(reinterpret_cast<char*>(&m_ePrimitiveTopology), sizeof(D3D11_PRIMITIVE_TOPOLOGY));
		
			is.read(reinterpret_cast<char*>(&m_iIndexFormat), sizeof(DXGI_FORMAT));
		
			is.read(reinterpret_cast<char*>(&m_iNumVertexBuffers), sizeof(_uint));
		
			is.read(reinterpret_cast<char*>(&m_iNumVertices), sizeof(_uint));
		
			is.read(reinterpret_cast<char*>(&m_iIndexStride), sizeof(_uint));
		
			is.read(reinterpret_cast<char*>(&m_iNumIndices), sizeof(_uint));
		
			is.read(reinterpret_cast<char*>(&m_iVertexStride), sizeof(_uint));
		
		
			is.read(reinterpret_cast<char*>(&m_BufferDesc.ByteWidth), sizeof(_uint));
			is.read(reinterpret_cast<char*>(&m_BufferDesc.Usage), sizeof(D3D11_USAGE));
			is.read(reinterpret_cast<char*>(&m_BufferDesc.BindFlags), sizeof(_uint));
			is.read(reinterpret_cast<char*>(&m_BufferDesc.CPUAccessFlags), sizeof(_uint));
			is.read(reinterpret_cast<char*>(&m_BufferDesc.MiscFlags), sizeof(_uint));
			is.read(reinterpret_cast<char*>(&m_BufferDesc.StructureByteStride), sizeof(_uint));
		
		
			m_iVertexStride= sizeof(VTXPHYSX);
			m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
			m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
			m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			m_BufferDesc.CPUAccessFlags = 0;
			m_BufferDesc.MiscFlags = 0;
			m_BufferDesc.StructureByteStride = m_iVertexStride;
		
		
		
		
		
		
			//인덱스 버퍼를 읽기 위해서 강제로 데이터를 읽어야 함./.
			CModel::MODELTYPE eModelType;
			is.read(reinterpret_cast<char*>(&eModelType), sizeof(CModel::MODELTYPE));
		
			_uint iNumBones = 0;
			is.read(reinterpret_cast<char*>(&iNumBones), sizeof(_uint));
		
			vector<_uint> vecBoneIndices;
			readVectorFromBinary(is, vecBoneIndices);
		
			vector<_float4x4> vecOffsetMatrices;
			readVectorFromBinary(is, vecOffsetMatrices);
		
		
			//정점 정보를 옮기기 위한 작업
			VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
			is.read(reinterpret_cast<char*>(pVertices), sizeof(VTXMESH) * m_iNumVertices);
		
		
		
		
			//실제 사용되는 정점 정보 저장
			VTXPHYSX* pPhysxVertices = new VTXPHYSX[m_iNumVertices];
			ZeroMemory(pPhysxVertices, sizeof(VTXPHYSX) * m_iNumVertices);

			m_pVertices= new VTXPHYSX[m_iNumVertices];
			ZeroMemory(m_pVertices, sizeof(VTXPHYSX) * m_iNumVertices);
		
			for (UINT i = 0; i < m_iNumVertices; i++)
			{
				m_pVertices[i].vPosition = pPhysxVertices[i].vPosition = pVertices[i].vPosition;
				m_pVertices[i].vColor= pPhysxVertices[i].vColor = _float3(0.0f, 1.0f, 0.0f);
			}
			m_InitialData.pSysMem = pPhysxVertices;
			if (FAILED(__super::Create_Buffer(&m_pVB)))
				return E_FAIL;
		
		
		
		
		
		
			Safe_Delete_Array(pVertices);
			Safe_Delete_Array(pPhysxVertices);
		
			
		
		
			is.read(reinterpret_cast<char*>(&m_BufferDesc.ByteWidth), sizeof(_uint));
			is.read(reinterpret_cast<char*>(&m_BufferDesc.Usage), sizeof(D3D11_USAGE));
			is.read(reinterpret_cast<char*>(&m_BufferDesc.BindFlags), sizeof(_uint));
			is.read(reinterpret_cast<char*>(&m_BufferDesc.CPUAccessFlags), sizeof(_uint));
			is.read(reinterpret_cast<char*>(&m_BufferDesc.MiscFlags), sizeof(_uint));
			is.read(reinterpret_cast<char*>(&m_BufferDesc.StructureByteStride), sizeof(_uint));
		
		
		
			_uint* pIndices = new _uint[m_iNumIndices];
			is.read(reinterpret_cast<char*>(pIndices), sizeof(_uint) * m_iNumIndices);

			m_pIndices = new _uint[m_iNumIndices];
			ZeroMemory(m_pIndices, sizeof(_uint) * m_iNumIndices);

			for (UINT i = 0; i < m_iNumIndices; ++i)
			{
				m_pIndices[i] = pIndices[i];
			}
		
		
		
			m_InitialData.pSysMem = pIndices;
		
			if (FAILED(__super::Create_Buffer(&m_pIB)))
				return E_FAIL;
		
		
		
		
		
			Safe_Delete_Array(pIndices);


	return S_OK;
}

CVIBuffer_PhysXBuffer * CVIBuffer_PhysXBuffer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const PxRenderBuffer& rRenderBuffer)
{
	CVIBuffer_PhysXBuffer*		pInstance = new CVIBuffer_PhysXBuffer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(rRenderBuffer)))
	{
		MSG_BOX("Failed To Created : CVIBuffer_PhysXBuffer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_PhysXBuffer* CVIBuffer_PhysXBuffer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const vector<_float3> vecVertices)
{
	CVIBuffer_PhysXBuffer* pInstance = new CVIBuffer_PhysXBuffer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(vecVertices)))
	{
		MSG_BOX("Failed To Created : CVIBuffer_PhysXBuffer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_PhysXBuffer* CVIBuffer_PhysXBuffer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, istream& is)
{
	CVIBuffer_PhysXBuffer* pInstance = new CVIBuffer_PhysXBuffer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(is)))
	{
		MSG_BOX("Failed To Created : CVIBuffer_PhysXBuffer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_PhysXBuffer* CVIBuffer_PhysXBuffer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const vector<_float3>& vecVertices, const std::vector<_uint>& vecIndices)
{
	CVIBuffer_PhysXBuffer* pInstance = new CVIBuffer_PhysXBuffer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(vecVertices, vecIndices)))
	{
		MSG_BOX("Failed To Created : CVIBuffer_PhysXBuffer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_PhysXBuffer::Clone(void * pArg)
{
	CVIBuffer_PhysXBuffer*		pInstance = new CVIBuffer_PhysXBuffer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_PhysXBuffer");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CVIBuffer_PhysXBuffer::Free()
{
	__super::Free();


	Safe_Delete_Array(m_pVertices);
	Safe_Delete_Array(m_pIndices);


}
