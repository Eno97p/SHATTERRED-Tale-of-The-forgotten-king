#include "stdafx.h"

#include "BlastWall.h"


#include"GameInstance.h"
#include "EffectManager.h"
#include "SideViewCamera.h"

CBlastWall::CBlastWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBlastWall::CBlastWall(const CBlastWall& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlastWall::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CBlastWall::Initialize(void* pArg)
{
	CBlastWall_DESC* Desc = static_cast<CBlastWall_DESC*>(pArg);


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (Desc != nullptr)
	{

		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&Desc->fWorldMatrix));
		m_vInitialPos ={ Desc->fWorldMatrix._41, Desc->fWorldMatrix._42, Desc->fWorldMatrix._43, 1.f };
	}

	//m_pTransformCom->Set_WorldMatrix()
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(75.f, 453.f, 98.f, 1.0f));
	//m_pTransformCom->Scaling(0.5f, 0.5f, 0.5f);

	if (FAILED(Add_Components()))
		return E_FAIL;


	//m_vInitialPos = 
	return S_OK;
}

void CBlastWall::Priority_Tick(_float fTimeDelta)
{
}

void CBlastWall::Tick(_float fTimeDelta)
{
	//for (size_t i = 0; i < m_iNumMeshes; i++)
		//m_pPhysXCom[i]->Tick(m_pTransformCom->Get_WorldFloat4x4());//Update_Collider(m_pTransformCom->Get_WorldMatrix());
	if(m_bIsHit)
		m_fLifeTime += fTimeDelta;

	if(m_fLifeTime > 3.0f)
		m_pGameInstance->Erase(this);


	if (m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.0f))
	{
		CComponent* pPlayerCharacter = m_pGameInstance->Get_Component(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_PhysX"));
		if(pPlayerCharacter == nullptr)
			return;
		//dynamic_cast<CPhysXComponent_Character*>(pPlayerCharacter)->GetData()->pController;

		PxController* pController = dynamic_cast<CPhysXComponent_Character*>(pPlayerCharacter)->Get_Controller();

		PxCapsuleController* pCapsuleController = static_cast<PxCapsuleController*>(pController);
		PxCapsuleGeometry capsuleGeom(pCapsuleController->getRadius(), pCapsuleController->getHeight() * 0.5f);
		PxVec3 vPlayerVec3(static_cast<_float>(pCapsuleController->getFootPosition().x), static_cast<_float>(pCapsuleController->getFootPosition().y), static_cast<_float>(pCapsuleController->getFootPosition().z));
		
		PxTransform pPlayerTransform = PxTransform(vPlayerVec3);
		
		//pController.get
	


		if (!m_bIsHit)
		{
			for (size_t i = 0; i < m_iNumMeshes; i++)
			{

				PxRigidDynamic* pActor = m_pPhysXCom[i]->Get_Actor()->is<PxRigidDynamic>();
				PxShape* pShape;
				pActor->getShapes(&pShape, 1);


				const PxGeometryHolder geometry = pShape->getGeometry();
				const PxConvexMeshGeometry& convexGeom = geometry.convexMesh();

				PxTransform wallPos = pActor->getGlobalPose();

				PxVec3 vSweepDir = wallPos.p - pPlayerTransform.p;
				vSweepDir.normalize();

				PxReal maxDist = 0.01f;
				PxGeomSweepHit sweepHit;
				PxGeometryQuery;
				//m_pGameInstance->GetScene()->sweep()
				_bool  bIsHit = PxGeometryQuery::sweep(
					vSweepDir,
					maxDist,
					capsuleGeom,
					pPlayerTransform,
					convexGeom,
					wallPos,
					sweepHit
				);

				if (bIsHit)
				{
					//
					_float4 ParticlePos;
					XMStoreFloat4(&ParticlePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
					EFFECTMGR->Generate_Particle(26, ParticlePos);
					EFFECTMGR->Generate_Particle(56, ParticlePos, nullptr, XMVectorSet(0.f,1.f,0.f,0.f), 90.f);
					EFFECTMGR->Generate_Particle(56, ParticlePos);
					Broken_Wall();
					m_pGameInstance->Disable_Echo();
					m_pGameInstance->Play_Effect_Sound(TEXT("BlastWall.mp3"), SOUND_EFFECT);
					m_bIsHit = true;
					dynamic_cast<CSideViewCamera*>(m_pGameInstance->Get_Cameras()[CAM_SIDEVIEW])->Shake_Camera(1.f, 0.2f);

					break;
				}

			}
		}
	}

	//PxSweep
}

void CBlastWall::Late_Tick(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.0f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
		for (size_t i = 0; i < m_iNumMeshes; i++)
			m_pPhysXCom[i]->Late_Tick(&m_vecMeshsTransforms[i]);
		if (m_pGameInstance->Get_MoveShadow())
		{
			m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
		}
	}


}

HRESULT CBlastWall::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_vecMeshsTransforms[i])))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

	/*	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;*/

		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
		//	return E_FAIL;



		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}
#pragma region 모션블러
	m_PrevWorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러
	return S_OK;

}

HRESULT CBlastWall::Add_Components()
{

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fragile_Rock"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	m_iNumMeshes = m_pModelCom->Get_NumMeshes();
	m_pPhysXCom = new CPhysXComponent* [m_iNumMeshes];
	ZeroMemory(m_pPhysXCom, sizeof(CPhysXComponent*) * m_iNumMeshes);
	m_vecMeshsTransforms.resize(m_iNumMeshes);
	for(size_t i = 0; i < m_iNumMeshes; i++)
	{
		//m_vecMeshsTransforms.push_back(new _float4x4);
		XMStoreFloat4x4(&m_vecMeshsTransforms[i], m_pTransformCom->Get_WorldMatrix());
		wstring idxStr = to_wstring(i);

		CPhysXComponent::PHYSX_DESC		PhysXDesc;
		PhysXDesc.eGeometryType = PxGeometryType::eCONVEXMESH;
		PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);
		PhysXDesc.pMesh = m_pModelCom->Get_Meshes()[i];
		PhysXDesc.pName = "Wall";
		XMStoreFloat4x4(&PhysXDesc.fWorldMatrix, m_pTransformCom->Get_WorldMatrix());

		/* For.Com_Physx */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx"),
			TEXT("Com_PhysX") + idxStr, reinterpret_cast<CComponent**>(&m_pPhysXCom[i]),&PhysXDesc)))
			return E_FAIL;

		m_pPhysXCom[i]->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
		m_pPhysXCom[i]->Get_Actor()->userData = this;

		
		

	}



	return S_OK;
}

HRESULT CBlastWall::Bind_ShaderResources()
{
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
#pragma region 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_PrevViewMatrix)))
		return E_FAIL;
	_bool bMotionBlur = m_pGameInstance->Get_MotionBlur() || m_bMotionBlur;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MotionBlur", &bMotionBlur, sizeof(_bool))))
		return E_FAIL;
#pragma endregion 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CBlastWall::Check_Collision()
{

	PxShape* pShape;
	m_pPhysXCom[0]->Get_Actor()->is<PxRigidDynamic>()->getShapes(&pShape, 1);
	PxConvexMeshGeometry geom;
	const PxGeometryHolder geometry = pShape->getGeometry();
	geometry.convexMesh();



}

void CBlastWall::Broken_Wall()
{

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{

		m_pPhysXCom[i]->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
		m_pPhysXCom[i]->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
		//m_pPhysXCom[i]->Get_Actor()
		PxRigidDynamic* pRigidDynamic = m_pPhysXCom[i]->Get_Actor()->is<PxRigidDynamic>();
		pRigidDynamic->setMass(1.0f);
		pRigidDynamic->addForce(PxVec3(0.0f, -9.8f, 0.0f));
		//pRigidDynamic->setLinearVelocity(PxVec3(0.0f, 10.0f, 0.0f));


		int test = 0;

	}

}

CBlastWall* CBlastWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBlastWall* pInstance = new CBlastWall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBlastWall");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CBlastWall::Clone(void* pArg)
{
	CBlastWall* pInstance = new CBlastWall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBlastWall");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CBlastWall::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);

	for(size_t i = 0; i < m_iNumMeshes; i++)
		Safe_Release(m_pPhysXCom[i]);
	Safe_Delete_Array(m_pPhysXCom);
	//Safe_Release(m_pPhysXCom);
}
