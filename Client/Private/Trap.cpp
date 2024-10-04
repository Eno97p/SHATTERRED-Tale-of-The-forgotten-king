#include "Trap.h"
#include "Player.h"

#include "GameInstance.h"
#include "EffectManager.h"
#include "SideViewCamera.h"

CTrap::CTrap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMap_Element(pDevice, pContext)
	
{
}

CTrap::CTrap(const CTrap& rhs)
	: CMap_Element(rhs)
	
	
{
}

HRESULT CTrap::Initialize_Prototype()
{
	//int* Tmep = new int(0);
	return S_OK;
}

HRESULT CTrap::Initialize(void* pArg)
{
	TRAP_DESC* pDesc = static_cast<TRAP_DESC*>(pArg);

	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_dStartTime = pDesc->dStartTimeOffset;

	if (FAILED(Add_Components(pDesc)))
		return E_FAIL;

	m_pModelCom->Play_Animation(0.01f);

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->mWorldMatrix));
	
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(0, true));

	//145,522,98
	m_ColliderMat = XMLoadFloat4x4(m_pBoneMatrix);

	m_ColliderMat.r[0] = XMVector3Normalize(m_ColliderMat.r[0]);
	m_ColliderMat.r[1] = XMVector3Normalize(m_ColliderMat.r[1]);
	m_ColliderMat.r[2] = XMVector3Normalize(m_ColliderMat.r[2]);

	//_uint temp= m_pModelCom->Get_NumAnim();


	m_ColliderMat *= m_pTransformCom->Get_WorldMatrix();
	m_pColliderCom->Tick(m_ColliderMat);

	return S_OK;
}

void CTrap::Priority_Tick(_float fTimeDelta)
{

}

void CTrap::Tick(_float fTimeDelta)
{

	m_ColliderMat = XMLoadFloat4x4(m_pBoneMatrix);

	m_ColliderMat.r[0] = XMVector3Normalize(m_ColliderMat.r[0]);
	m_ColliderMat.r[1] = XMVector3Normalize(m_ColliderMat.r[1]);
	m_ColliderMat.r[2] = XMVector3Normalize(m_ColliderMat.r[2]);

	//_uint temp= m_pModelCom->Get_NumAnim();


	m_ColliderMat *= m_pTransformCom->Get_WorldMatrix();
	m_pColliderCom->Tick(m_ColliderMat);

	if (IsPillar)
	{
		if (m_pModelCom->Check_CurDuration(0.31))
		{
			_float4 vPartPos;
			XMStoreFloat4(&vPartPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			EFFECTMGR->Generate_Particle(30, vPartPos);
			EFFECTMGR->Generate_Particle(42, vPartPos);
			EFFECTMGR->Generate_Particle(43, vPartPos);
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Smashing.mp3"), SOUND_EFFECT);
		}
	}


	m_pModelCom->Play_Animation(fTimeDelta * m_fTimeAccel);
}

void CTrap::Late_Tick(_float fTimeDelta)
{
//#ifdef _DEBUG
//	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
//#endif
		// 몬스터 무기와 플레이어 충돌처리
	m_pColliderCom->Tick(m_ColliderMat);

	m_eColltype = m_pColliderCom->Intersect(m_pPlayer->Get_Collider());
	if (m_eColltype == CCollider::COLL_START)
	{
		m_pPlayer->PlayerHit(10);
		dynamic_cast<CSideViewCamera*>(m_pGameInstance->Get_Cameras()[CAM_SIDEVIEW])->Shake_Camera(0.25f, 0.2f);

	}

	if (m_pGameInstance->isIn_WorldFrustum(m_ColliderMat.r[3], 10.f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	}


#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CTrap::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;



	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) // 해당 Model의 Mesh만큼 순회
	{
		m_pShaderCom->Unbind_SRVs();

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;


		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS)))
		//	return E_FAIL;

		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MetalicTexture", i, aiTextureType_METALNESS)))
		//	return E_FAIL;

		//if ( i != 29)
		
		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Red", &i, sizeof(_uint))))
		//	return E_FAIL;

		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Test", &m_iTest, sizeof(_uint))))
		//	return E_FAIL;

		//if (i == 1)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
				return E_FAIL;
		}
		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

#pragma region 모션블러
	m_PrevWorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러

}

HRESULT CTrap::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	

		m_pShaderCom->Unbind_SRVs();

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", 0);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", 0, aiTextureType_EMISSIVE)))
			return E_FAIL;

		m_pShaderCom->Begin(6);

		m_pModelCom->Render(0);
}

HRESULT CTrap::Add_Components(TRAP_DESC* desc)
{
	

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, desc->wstrModelName.c_str(),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;





	if (desc->TriggerType == 0) //하꼬
	{
		m_pBoneMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("b_hachoir018");
		m_fTimeAccel = 1.2f;
		/* For.Com_Collider */
		CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

		ColliderDesc.eType = CCollider::TYPE_OBB;
		ColliderDesc.vExtents = _float3(1.f, 1.5f, 2.f);
		ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

		if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
			return E_FAIL;
		IsPillar = false;
	}
	else //스매싱필러
	{
		m_pBoneMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("B_smash_01_end");
		m_fTimeAccel = 3.f;
		m_dStartTime * 1.7f;

		/* For.Com_Collider */
		CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

		ColliderDesc.eType = CCollider::TYPE_AABB;
		ColliderDesc.vExtents = _float3(1.5f, 2.f, 2.f);
		ColliderDesc.vCenter = _float3(0.f, -ColliderDesc.vExtents.y * 2.f, 0.f);

		if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
			return E_FAIL;

		IsPillar = true;
	}

	m_pModelCom->Setting_StartTime(m_dStartTime);

	//CPhysXComponent::PHYSX_DESC		PhysXDesc{};
	//PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);
	//PhysXDesc.fBoxProperty = _float3(10.f, 1.f, 10.f);				//박스 크기
	//XMStoreFloat4x4(&PhysXDesc.fWorldMatrix, m_pTransformCom->Get_WorldMatrix());
	//XMStoreFloat4x4(&PhysXDesc.fOffsetMatrix, XMMatrixRotationX(XMConvertToRadians(0.0f)) * XMMatrixTranslation(0.f, 0.f, 0.f));  //오프셋 위치


	//PhysXDesc.pComponent = m_pModelCom;
	//PhysXDesc.eGeometryType = PxGeometryType::eBOX;
	//PhysXDesc.pName = "Elevator";
	//PhysXDesc.filterData.word0 = GROUP_ENVIRONMENT;
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx"),
	//	TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &PhysXDesc)))
	//	return E_FAIL;
	return S_OK;
}

HRESULT CTrap::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
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

	/*if (FAILED(m_pNoiseCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 7)))
		return E_FAIL;*/

	return S_OK;
}

CTrap* CTrap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrap* pInstance = new CTrap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTrap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrap::Clone(void* pArg)
{
	CTrap* pInstance = new CTrap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTrap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrap::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pPlayer);
	
}
