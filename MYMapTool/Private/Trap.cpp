#include "Trap.h"
#include "Player.h"

#include "GameInstance.h"
#include "ToolObj_Manager.h"
CTrap::CTrap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CToolObj(pDevice, pContext)
	
{
}

CTrap::CTrap(const CTrap& rhs)
	: CToolObj(rhs)
	
	
{
}

HRESULT CTrap::Initialize_Prototype()
{
	//int* Tmep = new int(0);
	return S_OK;
}

HRESULT CTrap::Initialize(void* pArg)
{

	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;

	if (nullptr != pArg)
	{

		TRAP_DESC* pDesc = (TRAP_DESC*)pArg;
		strcpy_s(m_szName, "Prototype_GameObject_Trap");
		strcpy_s(m_szLayer, "Layer_Trap");
		m_eModelType = CModel::TYPE_ANIM;
		m_iTriggerType = pDesc->TriggerType;
		m_dStartTime =  pDesc->dStartTimeOffset;

		if (m_iTriggerType == 0)
		{
			strcpy_s(m_szModelName, "Prototype_Component_Model_Hachoir");
		}
		else
		{
			strcpy_s(m_szModelName, "Prototype_Component_Model_SmashingPillar");
		}
		CToolObj_Manager::GetInstance()->Get_ToolObjs().emplace_back(this); // Obj


		// 생성 목록에 리스트 번호 매기기
		string strSize = to_string(CToolObj_Manager::GetInstance()->Get_ToolObjs().size()) + ". ";
		strcat_s(m_szListName, strSize.c_str());
		strcat_s(m_szListName, m_szName);
		CImgui_Manager::GetInstance()->Add_vecCreateObj(m_szListName);


		/*m_pTransformCom->Set_Scale(pDesc->mWorldMatrix.m[0][0], pDesc->mWorldMatrix.m[1][1], pDesc->mWorldMatrix.m[2][2]);

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(pDesc->mWorldMatrix.m[0][0], pDesc->mWorldMatrix.m[0][1], pDesc->mWorldMatrix.m[0][2], pDesc->mWorldMatrix.m[0][3]));
		m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(pDesc->mWorldMatrix.m[1][0], pDesc->mWorldMatrix.m[1][1], pDesc->mWorldMatrix.m[1][2], pDesc->mWorldMatrix.m[1][3]));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(pDesc->mWorldMatrix.m[2][0], pDesc->mWorldMatrix.m[2][1], pDesc->mWorldMatrix.m[2][2], pDesc->mWorldMatrix.m[2][3]));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3]);*/

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&((GRASS_DESC*)pArg)->vStartPos));

	/*	_vector vPos = XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3];
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);*/
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->mWorldMatrix));
		//TRIGGER STATE SET
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(145.f, 522.f, 98.f, 1.0f));
	
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(0, true));
	//145,522,98
	return S_OK;
}

void CTrap::Priority_Tick(_float fTimeDelta)
{

}

void CTrap::Tick(_float fTimeDelta)
{

	_matrix BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

	BoneMatrix.r[0] = XMVector3Normalize(BoneMatrix.r[0]);
	BoneMatrix.r[1] = XMVector3Normalize(BoneMatrix.r[1]);
	BoneMatrix.r[2] = XMVector3Normalize(BoneMatrix.r[2]);

	//_uint temp= m_pModelCom->Get_NumAnim();


	m_pColliderCom->Tick(BoneMatrix * m_pTransformCom->Get_WorldMatrix());



	
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CTrap::Late_Tick(_float fTimeDelta)
{
//#ifdef _DEBUG
//	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
//#endif


	//DECAL
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	//m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);

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

		//{
		//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
		//		return E_FAIL;
		//}
		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}
}

HRESULT CTrap::Render_Bloom()
{
	//if (FAILED(Bind_ShaderResources()))
	//	return E_FAIL;


	//_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	//

	//	m_pShaderCom->Unbind_SRVs();


	//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", 0, aiTextureType_EMISSIVE)))
	//		return E_FAIL;


	//	m_pShaderCom->Begin(6);

	//	m_pModelCom->Render(0);
	return S_OK;
}

HRESULT CTrap::Add_Components()
{
	


	wstring wstr;
	int len = MultiByteToWideChar(CP_ACP, 0, m_szModelName, -1, NULL, 0);
	if (len > 0)
	{
		wstr.resize(len);
		MultiByteToWideChar(CP_ACP, 0, m_szModelName, -1, &wstr[0], len);
		wstr.pop_back(); // 널 종료 문자 제거
	}

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, wstr.c_str(),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;




	if (m_iTriggerType == 0)
	{
		m_pBoneMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("b_hachoir018");
		/* For.Com_Collider */
		CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

		ColliderDesc.eType = CCollider::TYPE_OBB;
		ColliderDesc.vExtents = _float3(1.f, 1.5f, 2.f);
		ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

		if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
			return E_FAIL;
	}
	else
	{
		m_pBoneMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("B_smash_01_end");
		/* For.Com_Collider */
		CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

		ColliderDesc.eType = CCollider::TYPE_AABB;
		ColliderDesc.vExtents = _float3(1.5f, 2.f, 2.f);
		ColliderDesc.vCenter = _float3(0.f, -ColliderDesc.vExtents.y * 2.f, 0.f);

		if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
			return E_FAIL;
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
	
}
