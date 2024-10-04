#include "..\Public\PhysXComponent.h"


#include "Shader.h"
#include "GameInstance.h"
#include"GameObject.h"


#include"CPhysX.h"


#include"Mesh.h"



CPhysXComponent::CPhysXComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent { pDevice, pContext }
{
}

CPhysXComponent::CPhysXComponent(const CPhysXComponent & rhs)
	: CComponent{ rhs }
#ifdef _DEBUG
	, m_pShader{ rhs.m_pShader }
	, m_OutDesc{ rhs.m_OutDesc }
#endif
	, m_pBuffer{ rhs.m_pBuffer }
	, m_pMaterial{ rhs.m_pMaterial }
	, m_pActor{ rhs.m_pActor }
	, m_pShape{ rhs.m_pShape }

{
#ifdef _DEBUG
	Safe_AddRef(m_pShader);	
#endif

	for (auto& iter : m_pBuffer)
		Safe_AddRef(iter);
}

HRESULT CPhysXComponent::Initialize_Prototype()
{

#ifdef _DEBUG	
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PhysX.hlsl"), VTXPHYSX::Elements, VTXPHYSX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CPhysXComponent::Initialize(void * pArg)
{
	CPhysXComponent::PHYSX_DESC* pDesc = static_cast<CPhysXComponent::PHYSX_DESC*>(pArg);
	
	if (pDesc != nullptr)
	{
		m_pModelCom = dynamic_cast<CModel*>(pDesc->pComponent);
		m_pMeshCom = pDesc->pMesh;
		m_WorldMatrix = pDesc->fWorldMatrix;
		m_fBoxProperty = pDesc->fBoxProperty;
		m_fCapsuleProperty = pDesc->fCapsuleProperty;
		m_Mass = pDesc->fMass;
		PxFilterData filterData = pDesc->filterData;


		PxTransform pxTrans = Convert_DxMat_To_PxTrans(pDesc->fWorldMatrix,&m_fScale);
		PxTransform pxOffsetTrans = Convert_DxMat_To_PxTrans(pDesc->fOffsetMatrix);

		m_pMaterial = m_pGameInstance->GetPhysics()->createMaterial(pDesc->fMatterial.x, pDesc->fMatterial.y, pDesc->fMatterial.z);

		if(m_pActor==nullptr)
			CreateActor(pDesc->eGeometryType, pxTrans, pxOffsetTrans);

		MakeFilterData(filterData);
		//pDesc
		if (pDesc->pName)
			m_pActor->setName(pDesc->pName);
	}

	
#ifdef _DEBUG
	m_pActor->setActorFlag(PxActorFlag::eVISUALIZATION, true);	
	
#endif
	if (FAILED(m_pGameInstance->AddActor(m_pActor)))
		return E_FAIL;

	

	//!!! �������� ������ �׸��� ����  ���� ����
	//! 
#ifdef _DEBUG
	if (FAILED(Init_Buffer()))
		return E_FAIL;

	m_OutDesc.pMaterial = m_pMaterial;
#endif

	
	return S_OK;
}

#ifdef _DEBUG
HRESULT CPhysXComponent::Init_Buffer()
{

	//�ִ� 10��?
	//���ø޸� ���� ���� ������ �̵�
	PxShape** Shapes = new PxShape*[10];

	//���⼭ �����ε�? //�޾ƿ����� ������ ������ �ӵ��� ������?
	//���Ƶ� ���� ���� :Ȯ�� �Ϸ�
	PxU32 numShapes=	m_pActor->getShapes(Shapes, 10);



	for (PxU32 i = 0; i < numShapes; ++i)
	{
		PxShape* shape = Shapes[i];
	
		const PxGeometryHolder geometry = shape->getGeometry();
		
		switch (geometry.getType())
		{
		case PxGeometryType::eSPHERE:
		{

			break;
		}
		case PxGeometryType::ePLANE:
		{

			break;
		}
		case PxGeometryType::eCAPSULE:
		{
			PxCapsuleGeometry capsuleGeometry = geometry.capsule();
			_float fRadius = capsuleGeometry.radius;
			_float fHalfHeight = capsuleGeometry.halfHeight;

			m_vecVertices = CreateCapsuleVertices(fRadius, fHalfHeight);
			m_pBuffer.push_back(CVIBuffer_PhysXBuffer::Create(m_pDevice, m_pContext, m_vecVertices));
			break;
		}
		case PxGeometryType::eBOX:
		{
			PxBoxGeometry boxGeometry = geometry.box();
			PxVec3 halfExtents = boxGeometry.halfExtents;

			auto result = CreateBoxVertices(halfExtents);
			m_vecVertices = get<0>(result);
			vector<_uint> vecIndices = get<1>(result);
			
			m_pBuffer.push_back(CVIBuffer_PhysXBuffer::Create(m_pDevice, m_pContext, m_vecVertices, vecIndices));
			

			break;
		}
		case PxGeometryType::eCONVEXMESH:
		{
			PxConvexMeshGeometry	convexMeshGeometry = geometry.convexMesh();
			PxConvexMesh* convexMesh = convexMeshGeometry.convexMesh;
			

			break;
		}
		case PxGeometryType::ePARTICLESYSTEM:
		{
		

			break;
		}
		case PxGeometryType::eTETRAHEDRONMESH:
		{
			

			break;
		}
		case PxGeometryType::eTRIANGLEMESH:
		{
			PxTriangleMeshGeometry triangleMeshGeometry = geometry.triangleMesh();
			PxTriangleMesh* triangleMesh = triangleMeshGeometry.triangleMesh;
			if (nullptr == triangleMesh)
				return E_FAIL;

			const PxVec3* vertices = triangleMesh->getVertices();
			const PxU32 numVertices = triangleMesh->getNbVertices();


			const void* indexBuffer = triangleMesh->getTriangles();
			const PxU32 numTriangles = triangleMesh->getNbTriangles();
			_bool Is16Bit = triangleMesh->getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES;



			vector<_float3> vecVertices(numVertices);
			for (PxU32 i = 0; i < numVertices; ++i)
			{
				vecVertices[i] = _float3(vertices[i].x, vertices[i].y, vertices[i].z);
			}


			vector<_uint> vecIndices(numTriangles * 3);
			if (Is16Bit)
			{
				const PxU16* indices = reinterpret_cast<const PxU16*>(indexBuffer);
				for (PxU32 i = 0; i < numTriangles * 3; ++i)
				{
					vecIndices[i] = static_cast<_uint>(indices[i]);
				}
			}
			else
			{
				const PxU32* indices = reinterpret_cast<const PxU32*>(indexBuffer);
				for (PxU32 i = 0; i < numTriangles * 3; ++i)
				{
					vecIndices[i] = indices[i];
				}
			}


			m_pBuffer.push_back(CVIBuffer_PhysXBuffer::Create(m_pDevice, m_pContext, vecVertices, vecIndices));


			//������ �ٸ������� ����� ���� �ݺ��� �� �ʿ� ����
			//return S_OK;
			
			break;
		}
		case PxGeometryType::eHEIGHTFIELD:
		{
			

			break;
		}
		case PxGeometryType::eHAIRSYSTEM:
		{
			break;
		}
		case PxGeometryType::eCUSTOM:
		{
			

			break;
		}
		default:
			break;
		}
		
	
	}

	//�������� ������ ��Ÿ���� ������ ����
	//m_pBuffer.push_back(CVIBuffer_PhysXBuffer::Create(m_pDevice, m_pContext, m_pActor->getScene()->getRenderBuffer()));
	//for (auto& iter : m_pBuffer)
	//{
	//	if (nullptr == iter)
	//		return E_FAIL;
	//}
	

	delete[] Shapes;
	Shapes = nullptr;

	return S_OK;
}

#endif


void CPhysXComponent::Tick(const _float4x4 * pWorldMatrix)
{
	XMVECTOR position, rotation, scale;
	XMMatrixDecompose(&scale, &rotation, &position, XMLoadFloat4x4(pWorldMatrix));

	// ������ ������ ��ġ�� ȸ���� ����
	PxTransform newTransform(PxVec3(position.m128_f32[0], position.m128_f32[1], position.m128_f32[2]), PxQuat(rotation.m128_f32[0], rotation.m128_f32[1], rotation.m128_f32[2], rotation.m128_f32[3]));
	m_pActor->setGlobalPose(newTransform);
	m_WorldMatrix= *pWorldMatrix;


}

void CPhysXComponent::Late_Tick(_float4x4* pWorldMatrix)
{
	
	PxTransform pTransform =	m_pActor->getGlobalPose();
	

	_float3 fPos = { pTransform.p.x,pTransform.p.y,pTransform.p.z };
	_float4 fRot = { pTransform.q.x,pTransform.q.y,pTransform.q.z,pTransform.q.w };

	XMVECTOR scale, rotation, translation;
	XMMATRIX worldMatrix = XMLoadFloat4x4(pWorldMatrix);
	XMMatrixDecompose(&scale, &rotation, &translation, worldMatrix);

	XMVECTOR physxPosition = XMVectorSet(fPos.x, fPos.y, fPos.z, 1.0f);
	XMVECTOR physxRotation = XMVectorSet(fRot.x, fRot.y, fRot.z, fRot.w);


	XMMATRIX newWorldMatrix = XMMatrixTransformation(
		XMVectorZero(),          // Scaling origin
		XMQuaternionIdentity(),  // Scaling orientation quaternion
		scale,                   // �����Ǵ� �����ϸ� ��
		XMVectorZero(),          // Rotation origin
		physxRotation,           // PhysX ȸ�� ��
		physxPosition            // PhysX ��ġ ��
	);


	
	XMStoreFloat4x4(pWorldMatrix, newWorldMatrix);
	m_WorldMatrix = *pWorldMatrix;
	
	
}

void CPhysXComponent::SetFilterData(PxFilterData filterData)
{
	if (nullptr != m_pActor)
	{
		PxShape* pShape = nullptr;
		m_pActor->getShapes(&pShape, 1);
		pShape->setSimulationFilterData(filterData);
		pShape->setQueryFilterData(filterData);
		


	}




}

#ifdef _DEBUG
HRESULT CPhysXComponent::Render()
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
//void* CPhysXComponent::GetData()
//{
//
//
//
//
//
//	return nullptr;
//}
#endif





vector<_float3> CPhysXComponent::CreateCapsuleVertices(float radius, float halfHeight, int segments, int rings)
{
	

	vector<_float3> vecVertices;




	//for (int j = 0; j <= rings; ++j)
	//{
	//	float phi = PxPi / 2 * j / rings;
	//	float y = radius * cosf(phi);
	//
	//	for (int i = 0; i <= segments; ++i)
	//	{
	//		float theta = i * PxPi * 2 / segments;
	//		float x = radius * cosf(theta) * sinf(phi);
	//		float z = radius * sinf(theta) * sinf(phi);
	//
	//		// ��� �ݱ� ���� �߰�
	//		vecVertices.emplace_back(x, y + halfHeight, z);
	//		// �ϴ� �ݱ� ���� �߰�
	//		vecVertices.emplace_back(x, -y - halfHeight, z);
	//	}
	//}



	//��� �ݱ�

	for (int j = 0; j <= rings; ++j)
	{
		for (int i = 0; i <= segments; ++i)
		{
			float theta = i * PxPi * 2 / segments;
			float phi = PxPi / 2 * j / rings;
			float x = radius * cosf(theta) * sinf(phi);
			float y = radius * cosf(phi) + halfHeight;
			float z = radius * sinf(theta) * sinf(phi);
			vecVertices.emplace_back(x, y, z);
		}
	}
	// �Ǹ��� �κ�
	for (int j = 0; j <= rings; ++j)
	{
		float y = (2.0f * halfHeight) * j / rings - halfHeight;
		for (int i = 0; i <= segments; ++i)
		{
			float theta = i * PxPi * 2 / segments;
			float x = radius * cosf(theta);
			float z = radius * sinf(theta);
			vecVertices.emplace_back(x, y, z);
		}
	}
	
	// �ϴ� �ݱ�
	for (int j = 0; j <= rings; ++j)
	{
		for (int i = 0; i <= segments; ++i)
		{
			float theta = i * PxPi * 2 / segments;
			float phi = PxPi / 2 * j / rings;
			float x = radius * cosf(theta) * sinf(phi);
			float y = -radius * cosf(phi)-halfHeight;
			float z = radius * sinf(theta) * sinf(phi);
			vecVertices.emplace_back(x, y, z);
		}
	}

	// ��� ������ �����Ͽ� ĸ���� �ϴ��� �� ���� ��ġ�ϵ��� �մϴ�.
	float offsetY = halfHeight + radius; // �� �������� ���� �Ÿ�
	for (size_t i = 0; i < vecVertices.size(); ++i)
	{
		vecVertices[i].y += offsetY;
	}

	return vecVertices;




	
}

tuple<vector<_float3>, vector<_uint>> CPhysXComponent::CreateBoxVertices(const PxVec3& halfExtents)
{
	std::vector<_float3> vertices;
	std::vector<_uint> indices;
	
	vertices.push_back(_float3(-halfExtents.x, -halfExtents.y, -halfExtents.z)); // 0
	vertices.push_back(_float3(halfExtents.x, -halfExtents.y, -halfExtents.z));  // 1
	vertices.push_back(_float3(-halfExtents.x, halfExtents.y, -halfExtents.z));  // 2
	vertices.push_back(_float3(halfExtents.x, halfExtents.y, -halfExtents.z));   // 3
	vertices.push_back(_float3(-halfExtents.x, -halfExtents.y, halfExtents.z));  // 4
	vertices.push_back(_float3(halfExtents.x, -halfExtents.y, halfExtents.z));   // 5
	vertices.push_back(_float3(-halfExtents.x, halfExtents.y, halfExtents.z));   // 6
	vertices.push_back(_float3(halfExtents.x, halfExtents.y, halfExtents.z));    // 7


	indices = {
		// �ո�
		0, 1, 2, 2, 1, 3,
		// �޸�
		4, 6, 5, 5, 6, 7,
		// ���ʸ�
		0, 2, 4, 4, 2, 6,
		// �����ʸ�
		1, 5, 3, 3, 5, 7,
		// ����
		2, 3, 6, 6, 3, 7,
		// �Ʒ���
		0, 4, 1, 1, 4, 5
	};


	return	make_tuple(vertices, indices);
	
}

HRESULT CPhysXComponent::CreateActor(PxGeometryType::Enum eGeometryType, const PxTransform& pxTrans, const PxTransform& pxOffsetTrans)
{

	switch (eGeometryType)
	{
	case physx::PxGeometryType::eSPHERE:
		break;
	case physx::PxGeometryType::ePLANE:
		break;
	case physx::PxGeometryType::eCAPSULE:
	{
		
		PxCapsuleGeometry temp = PxCapsuleGeometry(m_fCapsuleProperty.x, m_fCapsuleProperty.y);
		m_pActor = PxCreateDynamic(*m_pGameInstance->GetPhysics(), PxTransform(pxTrans), temp, *m_pMaterial, m_Mass == 0.0f ? 1.0f : m_Mass, pxOffsetTrans);
		
		if(nullptr==m_pActor)
			return E_FAIL;
		break;
	}
	case physx::PxGeometryType::eBOX:
	{

		PxBoxGeometry temp = PxBoxGeometry(m_fBoxProperty.x, m_fBoxProperty.y, m_fBoxProperty.z);
		m_pActor = PxCreateDynamic(*m_pGameInstance->GetPhysics(), PxTransform(pxTrans), temp, *m_pMaterial, m_Mass==0.0f ? 1.0f: m_Mass, pxOffsetTrans);
		if (nullptr == m_pActor)
			return E_FAIL;
		break;
	}
	case physx::PxGeometryType::eCONVEXMESH:
	{

		auto Meshresult = CreateTriangleMeshDesc(m_pMeshCom);
		vector<PxVec3> vecVertices = get<0>(Meshresult);
		vector<PxU32> vecIndices = get<1>(Meshresult);

		PxConvexMeshDesc convexDesc;
		convexDesc.points.count = static_cast<PxU32>(vecVertices.size());
		convexDesc.points.stride = sizeof(PxVec3);
		convexDesc.points.data = vecVertices.data();
		convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

		PxCookingParams cookingParams(m_pGameInstance->GetPhysics()->getTolerancesScale());
		cookingParams.midphaseDesc.setToDefault(PxMeshMidPhase::eBVH34);
		cookingParams.buildGPUData = true;
		cookingParams.meshWeldTolerance = 0.001f;
		cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
		cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eWELD_VERTICES;
		cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eFORCE_32BIT_INDICES;



		// Convex Mesh Cooking
		PxDefaultMemoryOutputStream convexWriteBuffer;
		PxConvexMeshCookingResult::Enum convexResult;
		bool bConvexSuccess = PxCookConvexMesh(cookingParams, convexDesc, convexWriteBuffer);
		if (!bConvexSuccess)
			return E_FAIL;
		

		PxDefaultMemoryInputData convexReadBuffer(convexWriteBuffer.getData(), convexWriteBuffer.getSize());
		PxConvexMesh* pConvexMesh = m_pGameInstance->GetPhysics()->createConvexMesh(convexReadBuffer);

		if (nullptr == pConvexMesh)
			return E_FAIL;

		PxConvexMeshGeometry convexGeom = PxConvexMeshGeometry(pConvexMesh);

		// Create a dynamic actor
		m_pActor = m_pGameInstance->GetPhysics()->createRigidDynamic(pxTrans);
		if (nullptr == m_pActor)
			return E_FAIL;

		PxShape* pShape = m_pGameInstance->GetPhysics()->createShape(convexGeom, *m_pMaterial, true);
		if (!m_pActor->attachShape(*pShape))
			return E_FAIL;

		pShape->release();

	}

		break;
	case physx::PxGeometryType::ePARTICLESYSTEM:
		break;
	case physx::PxGeometryType::eTETRAHEDRONMESH:
		break;
	case physx::PxGeometryType::eTRIANGLEMESH:

		//for (auto& pMesh : m_pModelCom->Get_Meshes())
		{
			auto Meshresult = CreateTriangleMeshDesc(m_pMeshCom);
			vector<PxVec3> vecVertices = get<0>(Meshresult);
			vector<PxU32> vecIndices = get<1>(Meshresult);

			PxTriangleMeshDesc meshDesc;
			meshDesc.points.count = static_cast<PxU32>(vecVertices.size());
			meshDesc.points.stride = sizeof(PxVec3);
			meshDesc.points.data = vecVertices.data();

			meshDesc.triangles.count = static_cast<PxU32>(vecIndices.size() / 3);
			meshDesc.triangles.stride = 3 * sizeof(PxU32);
			meshDesc.triangles.data = vecIndices.data();



			PxCookingParams cookingParams(m_pGameInstance->GetPhysics()->getTolerancesScale());
			cookingParams.midphaseDesc.setToDefault(PxMeshMidPhase::eBVH34);
			cookingParams.buildGPUData = true;
			cookingParams.meshWeldTolerance = 0.001f;
			cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
			cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eWELD_VERTICES;
			cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eFORCE_32BIT_INDICES;


			PxDefaultMemoryOutputStream writeBuffer;
			PxTriangleMeshCookingResult::Enum result;
			bool bSuccess = PxCookTriangleMesh(cookingParams, meshDesc, writeBuffer, &result);
			if (!bSuccess)
				return E_FAIL;

			

			PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
			PxTriangleMesh* pTriangleMesh = m_pGameInstance->GetPhysics()->createTriangleMesh(readBuffer);

			if (nullptr == pTriangleMesh)
				return E_FAIL;

			PxTriangleMeshGeometry triGeom = PxTriangleMeshGeometry(pTriangleMesh);

			m_pActor = m_pGameInstance->GetPhysics()->createRigidDynamic(pxTrans);
			if (nullptr == m_pActor)
				return E_FAIL;
			PxShape* pShape = m_pGameInstance->GetPhysics()->createShape(triGeom, *m_pMaterial, true);
			if(!m_pActor->attachShape(*pShape))
				return E_FAIL;
				

			pShape->release();


			
		}
		


		break;
	case physx::PxGeometryType::eHEIGHTFIELD:
		break;
	case physx::PxGeometryType::eHAIRSYSTEM:
		break;
	case physx::PxGeometryType::eCUSTOM:
		break;
	case physx::PxGeometryType::eGEOMETRY_COUNT:
		break;
	case physx::PxGeometryType::eINVALID:
		break;
	default:
		break;
	}

	return S_OK;
}

_bool CPhysXComponent::RayCast(_vector vYourPos, _vector vYourDir, PxReal distance, const PxQueryFilterData& filterData, PxQueryFilterCallback* filterCall)
{
	//BVH�� ����ϸ� �� ������ �˻� ���� //���߿� �ٲٱ� 


	PxVec3 origin(vYourPos.m128_f32[0], vYourPos.m128_f32[1], vYourPos.m128_f32[2]);
	PxVec3 direction(vYourDir.m128_f32[0], vYourDir.m128_f32[1], vYourDir.m128_f32[2]);

	PxRaycastBuffer  hit;
	_bool status = m_pGameInstance->GetScene()->raycast(origin, direction, distance, hit, PxHitFlag::eDEFAULT, filterData, filterCall);

	if (status)
	{

		//���߿� ��������Ÿ�� ������ 
		//hit.
	}



	return status;
}





void CPhysXComponent::MakeFilterData(PxFilterData& filterData)
{
	if (filterData.word0 != 0 || filterData.word1 != 0 || filterData.word2 != 0 || filterData.word3 != 0)
	{
		PxU32 numShapes = m_pActor->getNbShapes();
		PxShape** shapes = new PxShape* [numShapes];
		m_pActor->getShapes(shapes, numShapes);
		
		for (PxU32 i = 0; i < numShapes; i++)
		{
			shapes[i]->setSimulationFilterData(filterData);
			//shapes[i]->setQueryFilterData(filterData);
		}

		delete[] shapes;
		shapes= nullptr;
	}

}

tuple<vector<PxVec3>, vector<PxU32>> CPhysXComponent::CreateTriangleMeshDesc(void* pvoid)
{
	CMesh* pMesh =	static_cast<CMesh*>(pvoid);

	vector<PxVec3> vecVertices(pMesh->Get_NumVertex());
	for(_uint i = 0; i < pMesh->Get_NumVertex(); ++i)
	{
		vecVertices[i] = PxVec3(
			pMesh->Get_Vertices()[i].vPosition.x * m_fScale.x,
			pMesh->Get_Vertices()[i].vPosition.y * m_fScale.y,
			pMesh->Get_Vertices()[i].vPosition.z * m_fScale.z);

	}

	vector<PxU32> vecIndices(pMesh->Get_NumIndices());
	for(_uint i = 0; i < pMesh->Get_NumIndices(); ++i)
	{
		vecIndices[i] = PxU32(pMesh->Get_Mesh_Indices()[i]);
	}


	return make_tuple(vecVertices, vecIndices);
}

CPhysXComponent * CPhysXComponent::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPhysXComponent*		pInstance = new CPhysXComponent(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPhysXComponent");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CComponent * CPhysXComponent::Clone(void * pArg)
{
	CPhysXComponent*		pInstance = new CPhysXComponent(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPhysXComponent");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CPhysXComponent::Free()
{
__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif
	for (auto& pPhysXBuffer : m_pBuffer)
		Safe_Release(pPhysXBuffer);

	if (m_pMaterial)
	{ 
		if(m_pMaterial->isReleasable())
			Safe_physX_Release(m_pMaterial);

	}

	if (m_pActor)
	{
		if (m_pActor->getScene() == m_pGameInstance->GetScene())
			m_pGameInstance->GetScene()->removeActor(*m_pActor);
		
	}

	int tmep = 0;


}

PxTransform CPhysXComponent::Convert_DxMat_To_PxTrans(const _float4x4& pWorldMatrix, _float3* fSclae)
{



	_vector vScale, vRotation, vPosition;
	XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&pWorldMatrix));

	if(fSclae!=nullptr)
		XMStoreFloat3(fSclae, vScale);

	_float4 fPos;
	XMStoreFloat4(&fPos, vPosition);

	_float4 fRotQuat;
	XMStoreFloat4(&fRotQuat, vRotation);
	PxQuat fRot = PxQuat(fRotQuat.x, fRotQuat.y, fRotQuat.z, fRotQuat.w);




	PxTransform pxTrans = PxTransform(PxVec3(fPos.x, fPos.y, fPos.z), fRot);
	
	return pxTrans;
}

XMMATRIX CPhysXComponent::Convert_PxTrans_To_DxMat(const PxTransform& pTrans)
{
	PxVec3 position = pTrans.p;
	PxQuat rotation = pTrans.q;

	XMVECTOR dxPosition = XMVectorSet(position.x, position.y, position.z, 1.0f);
	XMVECTOR dxRotation = XMVectorSet(rotation.x, rotation.y, rotation.z, rotation.w);

	XMVECTOR dxScale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);	// �������� PhysX Transform�� ���ԵǾ� ���� �����Ƿ�  1�� ���� 


	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(dxRotation);

	//ũ�� x ȸ�� x ��ġ ������ ���ص� ������  Direcx�� ���ɺ�ȯ�Լ��� �̿��ϸ� ����ȭ �Ǿ� �־ �̰��� ���

	XMMATRIX worldMatrix = XMMatrixAffineTransformation(dxScale, XMVectorZero(), dxRotation, dxPosition);


	return worldMatrix;
}

PxVec4 CPhysXComponent::Convert_XMVec4_To_PxVec4(const _vector pXMVec4)
{



	return PxVec4();
}

XMVECTOR CPhysXComponent::Convert_PxVec4_To_XMVec4(const PxVec4& pPxVec4)
{
	return XMVECTOR();
}
