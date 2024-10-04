#include"PhysXComponent_HeightField.h"


#include "Shader.h"
#include "GameInstance.h"
#include"GameObject.h"

#include"Mesh.h"



CPhysXComponent_HeightField::CPhysXComponent_HeightField(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPhysXComponent{ pDevice, pContext }
{
}

CPhysXComponent_HeightField::CPhysXComponent_HeightField(const CPhysXComponent_HeightField & rhs)
	: CPhysXComponent{ rhs }
	,m_fMinHeight{rhs.m_fMinHeight}
	,m_fMaxHeight{rhs.m_fMaxHeight}
	,m_pHeightField{rhs.m_pHeightField}
	,m_fHeightScale{rhs.m_fHeightScale}
	,m_iNumVerticeX{rhs.m_iNumVerticeX}
	,m_iNumVerticeZ{rhs.m_iNumVerticeZ}
	

#ifdef _DEBUG
	,m_OutDesc{rhs.m_OutDesc}
#endif // _DEBUG
{
}

HRESULT CPhysXComponent_HeightField::Initialize_Prototype(const wstring& strPrototypeTag)
{
	CVIBuffer_Terrain* TerrainCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Prototype(m_pGameInstance->Get_CurrentLevel(), strPrototypeTag));
	if(!TerrainCom)
		return E_FAIL;
	m_iNumVerticeX = TerrainCom->GetNumVerticesX();
	m_iNumVerticeZ = TerrainCom->GetNumVerticesZ();
	_float4* TerrainPosition = TerrainCom->Get_VertexPositions();






	for(PxU32 s = 0 ; s < m_iNumVerticeX * m_iNumVerticeZ; s++)
	{

		m_fMinHeight = PxMin(m_fMinHeight, TerrainPosition[s].y);
		m_fMaxHeight = PxMax(m_fMaxHeight, TerrainPosition[s].y);
	}

	PxReal fdeltaHeight = m_fMaxHeight - m_fMinHeight;
	PxReal fquantization = (PxReal)0x7fff/* 32767*/;

	m_fHeightScale = PxMax(fdeltaHeight / fquantization, PX_MIN_HEIGHTFIELD_Y_SCALE);
	




	PxHeightFieldSample* pSamples = new PxHeightFieldSample[m_iNumVerticeX * m_iNumVerticeZ];
	for (PxU32 row = 0; row < m_iNumVerticeX; row++)
	{
		for (PxU32 col = 0; col < m_iNumVerticeZ; col++)
		{
			PxU32 sourceIndex = row * m_iNumVerticeX + col;
			PxU32 destIndex = col * m_iNumVerticeX + row;

	
			// 높이 값 계산
			PxI16 height = static_cast<PxI16>((TerrainPosition[sourceIndex].y - m_fMinHeight) / m_fHeightScale) - 32768;
			pSamples[destIndex].height = height; // destIndex를 row * m_iNumVerticeX + col로 설정
			pSamples[destIndex].materialIndex0 = 0;
			pSamples[destIndex].materialIndex1 = 0;

			////PxU32 index = row * m_iNumVerticeZ + col;
			//PxU32 flippedRow = m_iNumVerticeZ - 1 - row;
			//PxU32 sourceIndex = flippedRow * m_iNumVerticeX + col;
			//PxU32 destIndex = row * m_iNumVerticeX + col;



			//PxI16 height = static_cast<PxI16>((TerrainPosition[sourceIndex].y - m_fMinHeight) / m_fHeightScale) -32768;
			//pSamples[destIndex].height = height;
			//pSamples[destIndex].materialIndex0 = 0;
			//pSamples[destIndex].materialIndex1 = 0;


		}

	}

	PxHeightFieldDesc hfDesc;
	hfDesc.format = PxHeightFieldFormat::eS16_TM;
	hfDesc.nbColumns = m_iNumVerticeX;
	hfDesc.nbRows = m_iNumVerticeZ;
	hfDesc.samples.data = pSamples;
	hfDesc.samples.stride = sizeof(PxHeightFieldSample);

	PxPhysics* physics = m_pGameInstance->GetPhysics();
	m_pHeightField = PxCreateHeightField(hfDesc, physics->getPhysicsInsertionCallback());


	if (!m_pHeightField)
	{
		Safe_Delete_Array(pSamples);
		return E_FAIL; // 생성 실패 처리
	}





	Safe_Delete_Array(pSamples);


	if(FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	if (FAILED(Create_HeightField()))
		return E_FAIL;

	



	return S_OK;
}

HRESULT CPhysXComponent_HeightField::Initialize(void * pArg)
{
	CPhysXComponent::PHYSX_DESC* pDesc = static_cast<CPhysXComponent::PHYSX_DESC*>(pArg);




	const float terrainScaleX = 3.f;  
	const float terrainScaleZ = 3.f;
	const float heightScale = 1000.f;


	float offsetX = (m_iNumVerticeX - 1) * terrainScaleX * 0.5f;
	float offsetZ = (m_iNumVerticeZ - 1) * terrainScaleZ * 0.5f;


	PxPhysics* physics = m_pGameInstance->GetPhysics();

	
	PxHeightFieldGeometry hfGeom(m_pHeightField, PxMeshGeometryFlag::eTIGHT_BOUNDS, m_fHeightScale, terrainScaleX, terrainScaleZ);
	//PxMeshGeometryFlags hfFlags = 

	PxTransform transform(PxVec3(-offsetX, heightScale, -offsetZ)); // 위치 설정
	PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.6f); // 마찰계수, 반발계수 등을 적절히 설정


	m_pActor = PxCreateStatic(*physics, transform, hfGeom, *material);
	m_pActor->setName("HeightField");


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	
		

	return S_OK;
}

#ifdef _DEBUG
HRESULT CPhysXComponent_HeightField::Render()
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
HRESULT CPhysXComponent_HeightField::Create_HeightField()
{


	return S_OK;
}


CPhysXComponent_HeightField * CPhysXComponent_HeightField::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
{
	CPhysXComponent_HeightField*		pInstance = new CPhysXComponent_HeightField(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strPrototypeTag)))
	{
		MSG_BOX("Failed to Created : CPhysXComponent_HeightField");
		return nullptr;
	}

	return pInstance;
}

CComponent * CPhysXComponent_HeightField::Clone(void * pArg)
{
	CPhysXComponent_HeightField*		pInstance = new CPhysXComponent_HeightField(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPhysXComponent_HeightField");
		return nullptr;
	}

	return pInstance;
}

void CPhysXComponent_HeightField::Free()
{
	__super::Free();

	Safe_physX_Release(m_pHeightField);
	

	

}
