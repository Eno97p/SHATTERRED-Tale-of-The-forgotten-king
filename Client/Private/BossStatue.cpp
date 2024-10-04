#include "BossStatue.h"
#include "GameInstance.h"
#include "Player.h"
#include "Weapon.h"
#include "EffectManager.h"

#include "UI_ArrowSign.h"

CBossStatue::CBossStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActive_Element(pDevice, pContext)
{
}

CBossStatue::CBossStatue(const CBossStatue& rhs)
	: CActive_Element(rhs)
{
}

HRESULT CBossStatue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossStatue::Initialize(void* pArg)
{

	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		CMap_Element::MAP_ELEMENT_DESC* pDesc = (CMap_Element::MAP_ELEMENT_DESC*)pArg;
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->mWorldMatrix));
	}

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);
	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CBossStatue::Priority_Tick(_float fTimeDelta)
{

}

void CBossStatue::Tick(_float fTimeDelta)
{
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	// 플레이어 무기와 몬스터의 충돌 여부

	CWeapon* pPlayerWeapon = dynamic_cast<CWeapon*>(m_pPlayer->Get_Weapon());
	if (!pPlayerWeapon->Get_Active())
	{
		m_pColliderCom->Reset();
		m_eColltype = CCollider::COLL_NOCOLL;
	}
	else
	{
		m_eColltype = m_pColliderCom->Intersect(pPlayerWeapon->Get_Collider());
	}

	if (m_eColltype == CCollider::COLL_START && m_bActive)
	{
		m_pGameInstance->Disable_Echo();
		m_pGameInstance->Play_Effect_Sound(TEXT("Hit.ogg"), SOUND_MONSTER05, 0.f, 1.f, 0.3f);
		_float4 fPos;
		XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		EFFECTMGR->Generate_Lightning(2, fPos);
		m_bActive = false;
	}

	// Sign UI 관련 함수
	if(m_bActive && nullptr != m_pSignUI)
	{
		m_pSignUI->Tick(fTimeDelta);
	}
}

void CBossStatue::Late_Tick(_float fTimeDelta)
{
//#ifdef _DEBUG
//	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
//#endif

	//DECAL
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	if (m_bActive)
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);

		if (nullptr != m_pSignUI)
			m_pSignUI->Late_Tick(fTimeDelta);
	}
	if (m_pGameInstance->Get_NotMoveShadow())
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
	}

#ifdef _DEBUG

	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CBossStatue::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/*if (m_pGameInstance->Key_Down(DIK_UP))
	{
		m_iTest++;
	}*/


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) // 해당 Model의 Mesh만큼 순회
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MetalicTexture", i, aiTextureType_METALNESS)))
			return E_FAIL;


		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MetalicTexture", i, aiTextureType_METALNESS)))
			return E_FAIL;*/

		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Red", &i, sizeof(_uint))))
		//	return E_FAIL;

		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Test", &m_iTest, sizeof(_uint))))
		//	return E_FAIL;

	/*	if (i == 0)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
				return E_FAIL;
		}*/
		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

#pragma region 모션블러
	m_PrevWorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러
}

HRESULT CBossStatue::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//if (m_pGameInstance->Key_Down(DIK_9))
	//{
	//	m_iTest++;
	//}
	//if (m_pGameInstance->Key_Down(DIK_0))
	//{
	//	m_iTest--;
	//}


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	

		m_pShaderCom->Unbind_SRVs();


		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", 2, aiTextureType_DIFFUSE)))
			return E_FAIL;


		m_pShaderCom->Begin(6);

		m_pModelCom->Render(2);
}

HRESULT CBossStatue::Add_Components(void* pArg)
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(1.5f, 3.f, 1.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	CMap_Element::MAP_ELEMENT_DESC* pDesc = (CMap_Element::MAP_ELEMENT_DESC*)pArg;

	/*wstring wstr = pDesc->wstrModelName;*/

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BossStatue"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMapElement"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossStatue::Bind_ShaderResources()
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

HRESULT CBossStatue::Create_UI()
{
	CUI_ArrowSign::UI_ARROWSIGN_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pSignUI = dynamic_cast<CUI_ArrowSign*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ArrowSign"), &pDesc));
	if (nullptr == m_pSignUI)
		return E_FAIL;

	return S_OK;
}

CBossStatue* CBossStatue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossStatue* pInstance = new CBossStatue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBossStatue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossStatue::Clone(void* pArg)
{
	CBossStatue* pInstance = new CBossStatue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBossStatue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossStatue::Free()
{
	__super::Free();

	Safe_Release(m_pSignUI);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pColliderCom);
}
