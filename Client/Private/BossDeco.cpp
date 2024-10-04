#include "BossDeco.h"
#include "Player.h"

#include "GameInstance.h"

CBossDeco::CBossDeco(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMap_Element(pDevice, pContext)
	
{
}

CBossDeco::CBossDeco(const CBossDeco& rhs)
	: CMap_Element(rhs)
	
	
{
}

HRESULT CBossDeco::Initialize_Prototype()
{
	//int* Tmep = new int(0);
	return S_OK;
}

HRESULT CBossDeco::Initialize(void* pArg)
{
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MAP_ELEMENT_DESC* pDesc = static_cast<MAP_ELEMENT_DESC*>(pArg);

	if (FAILED(Add_Components(pDesc)))
		return E_FAIL;

	m_pModelCom->Play_Animation(0.01f);

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->mWorldMatrix));
	
	if (pDesc->wstrModelName == TEXT("Prototype_Component_Model_SkeletalChain"))
	{
		m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(1, true));
	}
	else if (pDesc->wstrModelName == TEXT("Prototype_Component_Model_Chaudron"))
	{
		m_bBloom = true;
		m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(3, true));
	}

	return S_OK;
}

void CBossDeco::Priority_Tick(_float fTimeDelta)
{
	if(m_bDissolve)
	{
		m_fDisolveValue -= fTimeDelta * 0.4f;
	}
}

void CBossDeco::Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta * m_fTimeAccel);
}

void CBossDeco::Late_Tick(_float fTimeDelta)
{
//#ifdef _DEBUG
//	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
//#endif
		// 몬스터 무기와 플레이어 충돌처리

	//if (m_pGameInstance->isIn_WorldFrustum(m, 5.f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	}

//wd
}

HRESULT CBossDeco::Render()
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

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
			return E_FAIL;
		
		m_pShaderCom->Begin(m_iShaderPath);

		m_pModelCom->Render(i);
	}
#pragma region 모션블러
	m_PrevWorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러
	return S_OK;
}

HRESULT CBossDeco::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	

		m_pShaderCom->Unbind_SRVs();

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", 0);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", 0, aiTextureType_EMISSIVE)))
			return E_FAIL;

		m_pShaderCom->Begin(15);

		m_pModelCom->Render(0);
}

void CBossDeco::Play_DeadAnimation()
{
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(0, true));
	m_bDissolve = true;
	m_iShaderPath = 7;
}

HRESULT CBossDeco::Add_Components(MAP_ELEMENT_DESC* desc)
{
	

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, desc->wstrModelName.c_str(),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	
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


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossDeco::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_DisolveValue", &m_fDisolveValue, sizeof(_float))))
		return E_FAIL;

	/*if (FAILED(m_pNoiseCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 7)))
		return E_FAIL;*/

	return S_OK;
}

CBossDeco* CBossDeco::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossDeco* pInstance = new CBossDeco(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBossDeco");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossDeco::Clone(void* pArg)
{
	CBossDeco* pInstance = new CBossDeco(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBossDeco");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossDeco::Free()
{
	__super::Free();
	Safe_Release(m_pDisolveTextureCom);
}
