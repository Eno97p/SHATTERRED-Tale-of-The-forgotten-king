#include "..\Public\PhysXComponent_static.h"


#include "Shader.h"
#include "GameInstance.h"
#include"GameObject.h"

#include"Mesh.h"



CPhysXComponent_static::CPhysXComponent_static(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPhysXComponent{ pDevice, pContext }
{
}

CPhysXComponent_static::CPhysXComponent_static(const CPhysXComponent_static & rhs)
	: CPhysXComponent{ rhs }
	, m_strFilePath{ rhs.m_strFilePath }
#ifdef _DEBUG
	,m_OutDesc{rhs.m_OutDesc}
#endif // _DEBUG

	, m_pTriangleMesh{rhs.m_pTriangleMesh}
{
}

HRESULT CPhysXComponent_static::Initialize_Prototype(const _char* pModelFilePath, const wstring& FilePath)
{
	m_strFilePath = pModelFilePath;

	if(FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	//decltype(auto) Load_data = Load_Data<_char[MAX_PATH], _char[MAX_PATH], _char[MAX_PATH], _float4x4, CModel::MODELTYPE>(FilePath);

	//_float4x4 WorldMatrix = get<3>(Load_data);


	//PxTransform pxTrans =Convert_DxMat_To_PxTrans(WorldMatrix);

	//m_pActor= m_pGameInstance->GetPhysics()->createRigidStatic(pxTrans);

	if (FAILED(Load_Buffer()))
		return E_FAIL;
	
	if(FAILED(Create_PhysX_TriAngleMesh()))
		return E_FAIL;

	//samples.
	
	

	



	return S_OK;
}

HRESULT CPhysXComponent_static::Initialize(void * pArg)
{
	CPhysXComponent::PHYSX_DESC* pDesc = static_cast<CPhysXComponent::PHYSX_DESC*>(pArg);

	PxTransform pxTrans = Convert_DxMat_To_PxTrans(pDesc->fWorldMatrix);

	_vector vScale, vRotation, vPosition;
	XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&pDesc->fWorldMatrix));
	_float3 fScale;
	XMStoreFloat3(&fScale, vScale);



	m_pActor = m_pGameInstance->GetPhysics()->createRigidStatic(pxTrans);
	m_pMaterial = m_pGameInstance->GetPhysics()->createMaterial(0.5f, 0.5f, 0.5f);
	PxTriangleMeshGeometry triGeom;
	PxMeshScale TriangleScale;
	for (auto& TriangleMesh : m_pTriangleMesh)
	{
		TriangleScale.scale = PxVec3(fScale.x, fScale.y, fScale.z);
		triGeom.triangleMesh = TriangleMesh;
		triGeom.scale = TriangleScale;
		//(TriangleMesh, PxMeshScale(PxVec3(fScale.x, fScale.y, fScale.z)));
		PxShape* shape = m_pGameInstance->GetPhysics()->createShape(triGeom, *m_pMaterial,true);
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		m_pActor->attachShape(*shape);
		shape->release();

	}


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	
	m_pActor->setGlobalPose(Convert_DxMat_To_PxTrans(pDesc->fWorldMatrix));
	//PxHeightFieldSample* 
	PxHeightFieldDesc hfDesc;
	hfDesc.format = PxHeightFieldFormat::eS16_TM;
	//hfDesc.flags
	
		

	return S_OK;
}

#ifdef _DEBUG
HRESULT CPhysXComponent_static::Render()
{
	if (m_OutDesc.bIsOnDebugRender)
	{
		m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);

		m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW));
		m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ));


		m_pShader->Begin(1);


		//Test: Render Call 
		//m_pBuffer[0]->Bind_Buffers();
		//m_pBuffer[0]->Render();
		for (auto& pPhysXBuffer : m_pBuffer)
		{
			if (nullptr != pPhysXBuffer)
			{
				pPhysXBuffer->Bind_Buffers();
				pPhysXBuffer->Render();
			}


		}
	}

	return S_OK;
}
#endif

#ifdef _DEBUG
//void* CPhysXComponent_static::GetData()
//{
//
//
//
//
//
//	return nullptr;
//}
#endif


HRESULT CPhysXComponent_static::Create_PhysX_TriAngleMesh()
{
	
	size_t iMeshCount = m_pBuffer.size();
	for (size_t i = 0; i < iMeshCount; ++i)
	{
		
		auto Meshresult = CreateTriangleMeshDesc(m_pBuffer[i]);
		vector<PxVec3> vecVertices = get<0>(Meshresult);
		vector<PxU32> vecIndices = get<1>(Meshresult);
		
		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = static_cast<PxU32>(vecVertices.size());
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.points.data = vecVertices.data();
		
		meshDesc.triangles.count = static_cast<PxU32>(vecIndices.size() / 3);
		meshDesc.triangles.stride = sizeof(PxU32) * 3;
		meshDesc.triangles.data = vecIndices.data();
		
		
		//if (vecVertices.size() <= 65536) {
		//	meshDesc.flags=PxMeshFlag::e16_BIT_INDICES;  // 16비트 인덱스 사용
		//}
		//else
		//{
		//	meshDesc.flags.clear(PxMeshFlag::e16_BIT_INDICES);  // 32비트 인덱스 사용
		//}
		
		
		
		
		PxCookingParams cookingParams(m_pGameInstance->GetPhysics()->getTolerancesScale());
		cookingParams.midphaseDesc.setToDefault(PxMeshMidPhase::eBVH34);
		cookingParams.buildGPUData = true;
		cookingParams.meshWeldTolerance = 0.001f;
		cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
		cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eWELD_VERTICES;
		cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eFORCE_32BIT_INDICES;

		//cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
		//cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
		//cookingParams.midphaseDesc.mBVH33Desc.meshCookingHint = PxMeshCookingHint::eSIM_PERFORMANCE;
		
		
		
		PxDefaultMemoryOutputStream writeBuffer;
		PxTriangleMeshCookingResult::Enum result;
		bool bSuccess = PxCookTriangleMesh(cookingParams, meshDesc, writeBuffer, &result);
		if (!bSuccess)
			return E_FAIL;

		/*wstring filePath(m_strFilePath.begin(), m_strFilePath.end());
		size_t pos = filePath.find(L".fbx");

		wstring AddName = L" _PhysX";
		if (pos != wstring::npos)
		{

			filePath.insert(pos, AddName);
		}

		PxU8* pMeshData = writeBuffer.getData();
		PxU32 iMeshSize = writeBuffer.getSize();
		
		Save_Data(filePath, pMeshData, iMeshSize);*/

		

		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		PxTriangleMesh* pTriangleMesh = m_pGameInstance->GetPhysics()->createTriangleMesh(readBuffer);
		
		m_pTriangleMesh.push_back(pTriangleMesh);
		//나중에 수정해야 할 수도 있음 현재 그냥 상수로 넣어놓음
		//맵 마다 다르게 재질을 설정해야 한다면 구조 수정!
		//m_pMaterial = m_pGameInstance->GetPhysics()->createMaterial(0.5f, 0.5f, 0.5f);
		//PxShape* shape = m_pGameInstance->GetPhysics()->createShape(PxTriangleMeshGeometry(pTriangleMesh), *m_pMaterial);
		//
		//
		//m_pActor->attachShape(*shape);
		//
		//
		//shape->release();



	}





	return S_OK;
}

HRESULT CPhysXComponent_static::Load_Buffer()
{
	string binaryFile = "../Bin/BinaryFile/";

	string filenamePart(m_strFilePath);
	size_t lastSlashIndex = filenamePart.find_last_of('/');
	if (lastSlashIndex != std::string::npos) {
		filenamePart = filenamePart.substr(lastSlashIndex + 1);
	}
	size_t lastDotIndex = filenamePart.find_last_of('.');
	if (lastDotIndex != std::string::npos) {
		filenamePart = filenamePart.substr(0, lastDotIndex) + ".bin";
	}
	binaryFile += filenamePart;
	ifstream inFile(binaryFile, std::ios::binary);
	if (!inFile.is_open())
	{
		MSG_BOX("Failed to open file");
		return E_FAIL;
	}
	else
	{
		_float4x4 tempMat;
		CModel::ANIMATION_DESC tempDesc;
		inFile.read(reinterpret_cast<char*>(&tempMat), sizeof(_float4x4));
		inFile.read(reinterpret_cast<char*>(&tempDesc), sizeof(CModel::ANIMATION_DESC));


		_uint iTempBoneCount = 0;
		inFile.read(reinterpret_cast<char*>(&iTempBoneCount), sizeof(_uint));
		for (_uint i = 0; i < iTempBoneCount; i++)
		{
			char szName[MAX_PATH] = "";
			_int TempIndex = 0;
			_float4x4 tempMat2;
			inFile.read(szName, sizeof(_char) * MAX_PATH);
			inFile.read((char*)(&TempIndex), sizeof(_int));
			inFile.read((char*)(&tempMat2), sizeof(_float4x4));


		}
		_uint iTempMeshCount = 0;
		inFile.read(reinterpret_cast<char*>(&iTempMeshCount), sizeof(_uint));
		for (_uint i = 0; i < iTempMeshCount; i++)
		{
			m_pBuffer.push_back(CVIBuffer_PhysXBuffer::Create(m_pDevice, m_pContext, inFile));
		}






	}




	return S_OK;
}

tuple<vector<PxVec3>, vector<PxU32>> CPhysXComponent_static::CreateTriangleMeshDesc(void* pvoid)
{
	CVIBuffer_PhysXBuffer* pBuffer = reinterpret_cast<CVIBuffer_PhysXBuffer*>(pvoid);

	vector<PxVec3> vecVertices(pBuffer->Get_NumVertex());
	for(_uint i=0;i<pBuffer->Get_NumVertex();++i)
	{
		vecVertices[i] =PxVec3(pBuffer->Get_Vertices()[i].vPosition.x, pBuffer->Get_Vertices()[i].vPosition.y, pBuffer->Get_Vertices()[i].vPosition.z);
	}

	vector<PxU32> vecIndices(pBuffer->Get_NumIndices());
	for (_uint i = 0; i < pBuffer->Get_NumIndices(); ++i)
	{
		vecIndices[i] = pBuffer->Get_Indices()[i];
	}


	return make_tuple(vecVertices, vecIndices);
}



CPhysXComponent_static * CPhysXComponent_static::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext,const _char* pModelFilePath, const wstring& FilePath)
{
	CPhysXComponent_static*		pInstance = new CPhysXComponent_static(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, FilePath)))
	{
		MSG_BOX("Failed to Created : CPhysXComponent_static");
		return nullptr;
	}

	return pInstance;
}

CComponent * CPhysXComponent_static::Clone(void * pArg)
{
	CPhysXComponent_static*		pInstance = new CPhysXComponent_static(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPhysXComponent_static");
		return nullptr;
	}

	return pInstance;
}

void CPhysXComponent_static::Free()
{
	__super::Free();


	

}
