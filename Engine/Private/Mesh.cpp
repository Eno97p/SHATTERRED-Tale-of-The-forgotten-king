#include "..\Public\Mesh.h"
#include "Bone.h"
#include "Transform.h"
#include "GameInstance.h"
CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{

}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer{ rhs }
{
}

HRESULT CMesh::Initialize(CModel::MODELTYPE eModelType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones)
{
	strcpy_s(m_szName, pAIMesh->mName.data);

	m_iMaterialIndex = pAIMesh->mMaterialIndex;

	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iNumVertexBuffers = 1;

	m_iNumVertices = pAIMesh->mNumVertices;

	m_iIndexStride = 4;
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iNumPrimitive = pAIMesh->mNumFaces;
#pragma region VERTEX_BUFFER 

	HRESULT			hr = eModelType == CModel::TYPE_NONANIM ? Ready_Vertices_For_NonAnimMesh(pAIMesh, PreTransformMatrix) : Ready_Vertices_For_AnimMesh(pAIMesh, Bones);
	m_ModelType = eModelType;

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER 
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;



	_uint		iNumIndices = { 0 };
	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	pNewIndices = new _uint[m_iNumIndices];
	ZeroMemory(pNewIndices, sizeof(_uint) * m_iNumIndices);

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		pIndices[iNumIndices++] = pNewIndices[iNumIndices] = pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = pNewIndices[iNumIndices] = pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = pNewIndices[iNumIndices] = pAIMesh->mFaces[i].mIndices[2];
	}

	/*memcpy(pNewIndices, pIndices, sizeof(_uint) * m_iNumIndices);*/

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

void CMesh::Fill_Matrices(vector<class CBone*>& Bones, _float4x4* pMeshBoneMatrices)
{
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&pMeshBoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * XMLoadFloat4x4(Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix()));
	}
}


HRESULT CMesh::Ready_Vertices_For_NonAnimMesh(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	m_MeshBufferdesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_MeshBufferdesc.Usage = D3D11_USAGE_DEFAULT;
	m_MeshBufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_MeshBufferdesc.CPUAccessFlags = 0;
	m_MeshBufferdesc.MiscFlags = 0;
	m_MeshBufferdesc.StructureByteStride = m_iVertexStride;

	VTXMESH* pNonAnimVT = new VTXMESH[m_iNumVertices];
	m_nonAnimpVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pNonAnimVT, sizeof(VTXMESH) * m_iNumVertices);
	ZeroMemory(m_nonAnimpVertices, sizeof(VTXMESH) * m_iNumVertices);


	_vector vSum = XMVectorZero();

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pNonAnimVT[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3)); //포지션 저장
		memcpy(&m_nonAnimpVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pNonAnimVT[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pNonAnimVT[i].vPosition), PreTransformMatrix));
		XMStoreFloat3(&m_nonAnimpVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&m_nonAnimpVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pNonAnimVT[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&m_nonAnimpVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));//노말벡터 저장
		XMStoreFloat3(&pNonAnimVT[i].vNormal,
			XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pNonAnimVT[i].vNormal), PreTransformMatrix)));
		XMStoreFloat3(&m_nonAnimpVertices[i].vNormal,
			XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&m_nonAnimpVertices[i].vNormal), PreTransformMatrix)));

		memcpy(&pNonAnimVT[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));	//texcoord
		memcpy(&m_nonAnimpVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pNonAnimVT[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));	//tangent
		memcpy(&m_nonAnimpVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));


		//메쉬의 중심점을 계산하기 위한 코드
		vSum = XMVectorAdd(vSum, XMLoadFloat3(&pNonAnimVT[i].vPosition));
	}

	_vector vCentor = XMVectorDivide(vSum, XMVectorReplicate(static_cast<float>(m_iNumVertices)));


	XMVECTOR vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR vRight = XMVector3Cross(vUp, XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
	XMVECTOR vLook = XMVector3Cross(vRight, vUp);

	/*memcpy(m_nonAnimpVertices, pNonAnimVT, sizeof(VTXMESH) * m_iNumVertices);*/

	m_InitialData.pSysMem = pNonAnimVT;

	if (FAILED(__super::Create_BufferZ(&m_pVB, &m_MeshBufferdesc)))
		return E_FAIL;


	Safe_Delete_Array(pNonAnimVT);
	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_AnimMesh(const aiMesh* pAIMesh, const vector<class CBone*>& Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	m_MeshBufferdesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_MeshBufferdesc.Usage = D3D11_USAGE_DEFAULT;
	m_MeshBufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_MeshBufferdesc.CPUAccessFlags = 0;
	m_MeshBufferdesc.MiscFlags = 0;
	m_MeshBufferdesc.StructureByteStride = m_iVertexStride;


	VTXANIMMESH* AinmVT = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(AinmVT, sizeof(VTXANIMMESH) * m_iNumVertices);

	m_AnimVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(m_AnimVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&AinmVT[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&m_AnimVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));

		memcpy(&AinmVT[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&m_AnimVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&AinmVT[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&m_AnimVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&AinmVT[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		memcpy(&m_AnimVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	/* 이 메시에 영향을 주는 뼈의 갯수를 가져온다. */
	m_iNumBones = pAIMesh->mNumBones;

	/* 뼈들을 순회하면서 각각의 뼈가 어떤 정점들에게 영향을 주는가를 캐치한다. */
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		/* pAIBone = i번째 뼈다 */
		aiBone* pAIBone = pAIMesh->mBones[i];

		/*pAIBone->mOffsetMatrix*/
		_float4x4		OffsetMatrix;
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.emplace_back(OffsetMatrix);

		_uint		iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
			{
				if (true == pBone->Compare_Name(pAIBone->mName.data))
					return true;

				++iBoneIndex;

				return false;
			});

		m_BoneIndices.emplace_back(iBoneIndex);

		/* pAIBone->mNumWeights : i번째 뼈가 몇개의 정점들에게 영향을 주는가? */
		/* j루프 : 이 뼈가 영향을 주는 정점들을 순회한다. */
		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			aiVertexWeight		VertexWeight = pAIBone->mWeights[j];

			/* VertexWeight.mVertexId : 이 뼈가 영향을 주는 정점들 중, j번째 정점의 인덱스 */
			/* pVertices[VertexWeight.mVertexId].vBlendIndices: 이 정점에게 영햐응ㄹ 주는 뼈의 인덱스를 최대 네개 저장한다. */
			if (0.0f == AinmVT[VertexWeight.mVertexId].vBlendWeights.x)
			{
				AinmVT[VertexWeight.mVertexId].vBlendIndices.x = i;
				m_AnimVertices[VertexWeight.mVertexId].vBlendIndices.x = i;
				AinmVT[VertexWeight.mVertexId].vBlendWeights.x = VertexWeight.mWeight;
				m_AnimVertices[VertexWeight.mVertexId].vBlendWeights.x = VertexWeight.mWeight;
			}

			else if (0.0f == AinmVT[VertexWeight.mVertexId].vBlendWeights.y)
			{
				AinmVT[VertexWeight.mVertexId].vBlendIndices.y = i;
				m_AnimVertices[VertexWeight.mVertexId].vBlendIndices.y = i;
				AinmVT[VertexWeight.mVertexId].vBlendWeights.y = VertexWeight.mWeight;
				m_AnimVertices[VertexWeight.mVertexId].vBlendWeights.y = VertexWeight.mWeight;
			}

			else if (0.0f == AinmVT[VertexWeight.mVertexId].vBlendWeights.z)
			{
				AinmVT[VertexWeight.mVertexId].vBlendIndices.z = i;
				m_AnimVertices[VertexWeight.mVertexId].vBlendIndices.z = i;
				AinmVT[VertexWeight.mVertexId].vBlendWeights.z = VertexWeight.mWeight;
				m_AnimVertices[VertexWeight.mVertexId].vBlendWeights.z = VertexWeight.mWeight;
			}

			else
			{
				AinmVT[VertexWeight.mVertexId].vBlendIndices.w = i;
				m_AnimVertices[VertexWeight.mVertexId].vBlendIndices.w = i;
				AinmVT[VertexWeight.mVertexId].vBlendWeights.w = VertexWeight.mWeight;
				m_AnimVertices[VertexWeight.mVertexId].vBlendWeights.w = VertexWeight.mWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;


		_uint		iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
			{
				if (true == pBone->Compare_Name(m_szName))
					return true;

				++iBoneIndex;

				return false;
			});

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.emplace_back(OffsetMatrix);

		m_BoneIndices.emplace_back(iBoneIndex);
	}

	m_InitialData.pSysMem = AinmVT;

	/*memcpy(m_AnimVertices, AinmVT, sizeof(VTXANIMMESH) * m_iNumVertices);*/
	if (FAILED(__super::Create_BufferZ(&m_pVB, &m_MeshBufferdesc)))
		return E_FAIL;

	Safe_Delete_Array(AinmVT);

	return S_OK;
}

HRESULT CMesh::Save_Mesh(ostream& os)
{
	os.write(m_szName, sizeof(_char) * MAX_PATH);

	os.write((char*)&m_iNumPrimitive, sizeof(_uint));
	os.write((char*)&m_iMaterialIndex, sizeof(_uint));
	os.write((char*)&m_ePrimitiveTopology, sizeof(D3D11_PRIMITIVE_TOPOLOGY));
	os.write((char*)&m_iIndexFormat, sizeof(DXGI_FORMAT));
	os.write((char*)&m_iNumVertexBuffers, sizeof(_uint));
	os.write((char*)&m_iNumVertices, sizeof(_uint));
	os.write((char*)&m_iIndexStride, sizeof(_uint));
	os.write((char*)&m_iNumIndices, sizeof(_uint));
	os.write((char*)&m_iVertexStride, sizeof(_uint));


	os.write((char*)&m_MeshBufferdesc.ByteWidth, sizeof(_uint));
	os.write((char*)&m_MeshBufferdesc.Usage, sizeof(D3D11_USAGE));
	os.write((char*)&m_MeshBufferdesc.BindFlags, sizeof(_uint));
	os.write((char*)&m_MeshBufferdesc.CPUAccessFlags, sizeof(_uint));
	os.write((char*)&m_MeshBufferdesc.MiscFlags, sizeof(_uint));
	os.write((char*)&m_MeshBufferdesc.StructureByteStride, sizeof(_uint));

	os.write((char*)&m_ModelType, sizeof(CModel::MODELTYPE));
	os.write((char*)&m_iNumBones, sizeof(_uint));
	writeVectorToBinary(m_BoneIndices, os);
	writeVectorToBinary(m_OffsetMatrices, os);
	if (m_ModelType == CModel::TYPE_NONANIM)
	{
		/*os.write(reinterpret_cast<const char*>(&m_iNumVertices), sizeof(size_t));*/
	/*	for (size_t i = 0; i < m_iNumVertices; ++i) {
			os.write((char*)(&m_nonAnimpVertices[i].vPosition), sizeof(_float3));
			os.write((char*)(&m_nonAnimpVertices[i].vNormal), sizeof(_float3));
			os.write((char*)(&m_nonAnimpVertices[i].vTexcoord), sizeof(_float2));
			os.write((char*)(&m_nonAnimpVertices[i].vTangent), sizeof(_float3));
		}*/

		os.write((char*)m_nonAnimpVertices, sizeof(VTXMESH) * m_iNumVertices);
	}
	else
	{
		//for (size_t i = 0; i < m_iNumVertices; ++i) {
		//	/*os.write(reinterpret_cast<const char*>(&m_iNumVertices), sizeof(size_t));*/
		//	os.write(reinterpret_cast<const char*>(&m_AnimVertices[i].vBlendIndices), sizeof(XMUINT4));
		//	os.write(reinterpret_cast<const char*>(&m_AnimVertices[i].vBlendWeights), sizeof(XMFLOAT4));
		//	os.write(reinterpret_cast<const char*>(&m_AnimVertices[i].vNormal), sizeof(_float3));
		//	os.write(reinterpret_cast<const char*>(&m_AnimVertices[i].vPosition), sizeof(_float3));
		//	os.write(reinterpret_cast<const char*>(&m_AnimVertices[i].vTangent.x), sizeof(_float3));
		//	os.write(reinterpret_cast<const char*>(&m_AnimVertices[i].vTexcoord), sizeof(_float2));
		//}

		os.write((char*)m_AnimVertices, sizeof(VTXANIMMESH) * m_iNumVertices);
	}

	os.write((char*)&m_BufferDesc.ByteWidth, sizeof(_uint));
	os.write((char*)&m_BufferDesc.Usage, sizeof(D3D11_USAGE));
	os.write((char*)&m_BufferDesc.BindFlags, sizeof(_uint));
	os.write((char*)&m_BufferDesc.CPUAccessFlags, sizeof(_uint));
	os.write((char*)&m_BufferDesc.MiscFlags, sizeof(_uint));
	os.write((char*)&m_BufferDesc.StructureByteStride, sizeof(_uint));
	os.write((char*)(pNewIndices), sizeof(_uint) * m_iNumIndices);



	return S_OK;

}

HRESULT CMesh::Load_Mesh(istream& is)
{
	is.read(m_szName, sizeof(_char) * MAX_PATH);
	is.read((char*)&m_iNumPrimitive, sizeof(_uint));

	is.read((char*)&m_iMaterialIndex, sizeof(_uint));
	is.read((char*)&m_ePrimitiveTopology, sizeof(D3D11_PRIMITIVE_TOPOLOGY));
	is.read((char*)&m_iIndexFormat, sizeof(DXGI_FORMAT));
	is.read((char*)&m_iNumVertexBuffers, sizeof(_uint));
	is.read((char*)&m_iNumVertices, sizeof(_uint));
	is.read((char*)&m_iIndexStride, sizeof(_uint));
	is.read((char*)&m_iNumIndices, sizeof(_uint));
	is.read((char*)&m_iVertexStride, sizeof(_uint));

	is.read((char*)&m_MeshBufferdesc.ByteWidth, sizeof(_uint));
	is.read((char*)&m_MeshBufferdesc.Usage, sizeof(D3D11_USAGE));
	is.read((char*)&m_MeshBufferdesc.BindFlags, sizeof(_uint));
	is.read((char*)&m_MeshBufferdesc.CPUAccessFlags, sizeof(_uint));
	is.read((char*)&m_MeshBufferdesc.MiscFlags, sizeof(_uint));
	is.read((char*)&m_MeshBufferdesc.StructureByteStride, sizeof(_uint));

	is.read((char*)&m_ModelType, sizeof(CModel::MODELTYPE));

	is.read((char*)&m_iNumBones, sizeof(_uint));
	readVectorFromBinary(is, m_BoneIndices);
	readVectorFromBinary(is, m_OffsetMatrices);

	if (m_ModelType == CModel::TYPE_NONANIM)
	{
		/*VTXMESH* tempVertices = new VTXMESH[m_iNumVertices];*/
		m_nonAnimpVertices = new VTXMESH[m_iNumVertices];
		is.read((char*)(m_nonAnimpVertices), sizeof(VTXMESH) * m_iNumVertices);
		/*memcpy(m_nonAnimpVertices, tempVertices, sizeof(VTXMESH) * m_iNumVertices);
		delete[] tempVertices;*/
		m_InitialData.pSysMem = m_nonAnimpVertices;
		if (FAILED(__super::Create_BufferZ(&m_pVB, &m_MeshBufferdesc)))
			return E_FAIL;

	}
	else
	{

		m_AnimVertices = new VTXANIMMESH[m_iNumVertices];
		is.read((char*)(m_AnimVertices), sizeof(VTXANIMMESH) * m_iNumVertices);
		/*	memcpy(m_AnimVertices, tempVertices, sizeof(VTXANIMMESH) * m_iNumVertices);
			delete[] tempVertices;*/
		m_InitialData.pSysMem = m_AnimVertices;
		if (FAILED(__super::Create_BufferZ(&m_pVB, &m_MeshBufferdesc)))
			return E_FAIL;
	}

	is.read((char*)&m_BufferDesc.ByteWidth, sizeof(_uint));
	is.read((char*)&m_BufferDesc.Usage, sizeof(D3D11_USAGE));
	is.read((char*)&m_BufferDesc.BindFlags, sizeof(_uint));
	is.read((char*)&m_BufferDesc.CPUAccessFlags, sizeof(_uint));
	is.read((char*)&m_BufferDesc.MiscFlags, sizeof(_uint));
	is.read((char*)&m_BufferDesc.StructureByteStride, sizeof(_uint));


	pNewIndices = new _uint[m_iNumIndices];
	is.read((char*)(pNewIndices), sizeof(_uint) * m_iNumIndices);


	m_InitialData.pSysMem = pNewIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODELTYPE eModelType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eModelType, pAIMesh, PreTransformMatrix, Bones)))
	{
		MSG_BOX("Failed To Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::LoadCreate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, istream& is)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);
	if (FAILED(pInstance->Load_Mesh(is)))
	{
		MSG_BOX("Failed To LoadCreated : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	return nullptr;
}

void CMesh::Free()
{
	if (m_ModelType == CModel::TYPE_NONANIM)
		Safe_Delete_Array(m_nonAnimpVertices);
	else
		Safe_Delete_Array(m_AnimVertices);
	Safe_Delete_Array(pNewIndices);
	__super::Free();

}

