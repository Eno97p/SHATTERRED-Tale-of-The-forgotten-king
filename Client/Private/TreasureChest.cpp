#include "TreasureChest.h"
#include "GameInstance.h"
#include "Inventory.h"
#include "Item.h"
#include "EffectManager.h"
#include "UI_Activate.h"

CTreasureChest::CTreasureChest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActive_Element(pDevice, pContext)
{
}

CTreasureChest::CTreasureChest(const CTreasureChest& rhs)
	: CActive_Element(rhs)
{
}

HRESULT CTreasureChest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTreasureChest::Initialize(void* pArg)
{


	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;




	if (nullptr != pArg)
	{

		CActive_Element::MAP_ELEMENT_DESC* pDesc = (CActive_Element::MAP_ELEMENT_DESC*)pArg;

	/*	pDesc->mWorldMatrix._41 = -8.3f;
		pDesc->mWorldMatrix._42 = 3.5f;
		pDesc->mWorldMatrix._43 = -2.4f;
		pDesc->mWorldMatrix._44 = 1.f;*/

		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->mWorldMatrix));

		m_eTreasureState = (TREASURE_COLOR)pDesc->TriggerType;
		switch (m_eTreasureState)
		{
		case TREASURE_NORMAL:
			m_iShaderPath = 0;
			m_iBloomShaderPath = 2;
			break;
		case TREASURE_EPIC:
			m_iShaderPath = 3;
			m_iBloomShaderPath = 4;
			m_TreasureColor = { 0.265282571f, 0.f, 0.5637f, 1.f };
			break;
		case TREASURE_CLOAKING:
			m_iShaderPath = 3;
			m_iBloomShaderPath = 4;
			m_TreasureColor = { 0.161764681f,  0.161764681f, 0.161764681f, 1.f };
			break;
		}
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(0, true));

	m_pModelCom->Play_Animation(0.001);

	if (FAILED(Create_Activate()))
		return E_FAIL;

	return S_OK;
}

void CTreasureChest::Priority_Tick(_float fTimeDelta)
{
}

void CTreasureChest::Tick(_float fTimeDelta)
{
	if (m_bChestOpened)
	{
		m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(0, false));
		m_pModelCom->Play_Animation(fTimeDelta, false);

		if (!m_bReward && m_pModelCom->Get_AnimFinished())
		{
			Drop_Item();
			m_bReward = true;
		}
	}
	else
		m_pActivateUI->Tick(fTimeDelta);
}

void CTreasureChest::Late_Tick(_float fTimeDelta)
{
//#ifdef _DEBUG
//	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
//#endif

	//DECAL
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);

	if (!m_bChestOpened)
	{
		CComponent* pComponent = m_pGameInstance->Get_Component(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Transform"));
		_vector vPlayerPos;
		_vector vSavePointPos;
		if (pComponent)
		{

			vPlayerPos = dynamic_cast<CTransform*>(pComponent)->Get_State(CTransform::STATE_POSITION);
			vSavePointPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			if (fabs(XMVectorGetX(XMVector4Length(vPlayerPos - vSavePointPos)) < 2.0f)) // 5.f
			{
				if (m_pGameInstance->Key_Down(DIK_F))
				{
					_float4 vParticlePos;
					XMStoreFloat4(&vParticlePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
					EFFECTMGR->Generate_Particle(124, vParticlePos);
					m_bChestOpened = true;
					m_pGameInstance->Disable_Echo();
					m_pGameInstance->Play_Effect_Sound(TEXT("OpenChest.ogg"), SOUND_EFFECT);
					//Drop_Item();
				}

				m_pActivateUI->Late_Tick(fTimeDelta);
			}
		}

		
	}
}

HRESULT CTreasureChest::Render()
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

		if (FAILED(m_pShaderCom->Bind_RawValue("g_EpicColor", &m_TreasureColor, sizeof(_float4))))
			return E_FAIL;

		//if ( i != 29)
		
		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Red", &i, sizeof(_uint))))
		//	return E_FAIL;

		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Test", &m_iTest, sizeof(_uint))))
		//	return E_FAIL;

		i == 2 ? m_pShaderCom->Begin(m_iShaderPath) : m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

#pragma region 모션블러
	m_PrevWorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러

}

HRESULT CTreasureChest::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Unbind_SRVs();



	m_pShaderCom->Unbind_SRVs();

	m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", 2);

	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", 2, aiTextureType_EMISSIVE)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_EpicColor", &m_TreasureColor, sizeof(_float4))))
		return E_FAIL;

	m_pShaderCom->Begin(m_iBloomShaderPath);

	m_pModelCom->Render(2);
}

HRESULT CTreasureChest::Add_Components(void* pArg)
{
	CActive_Element::MAP_ELEMENT_DESC* pDesc = (CActive_Element::MAP_ELEMENT_DESC*)pArg;

	
	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TreasureChest"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxTreasureChest"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CTreasureChest::Bind_ShaderResources()
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

HRESULT CTreasureChest::Create_Activate()
{
	CUI::UI_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;

	m_pActivateUI = dynamic_cast<CUI_Activate*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Activate"), &pDesc));
	if (nullptr == m_pActivateUI)
		return E_FAIL;

	return S_OK;
}

void CTreasureChest::Drop_Item()
{
	switch (m_eTreasureState)
	{
	case Client::CTreasureChest::TREASURE_NORMAL: // 랜덤한 아이템 획득
		CInventory::GetInstance()->Add_DropItem(static_cast<CItem::ITEM_NAME>(rand() % CItem::ITEM_END));
		break;
	case Client::CTreasureChest::TREASURE_EPIC: // 무기 획득
		CInventory::GetInstance()->Add_Weapon(CItemData::ITEMNAME_DURGASWORD);
		break;
	case Client::CTreasureChest::TREASURE_CLOAKING: // 클로킹 획득
		CInventory::GetInstance()->Add_Skill(CItemData::ITEMNAME_AKSHA);
		break;
	default:
		break;
	}
}

CTreasureChest* CTreasureChest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTreasureChest* pInstance = new CTreasureChest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTreasureChest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTreasureChest::Clone(void* pArg)
{
	CTreasureChest* pInstance = new CTreasureChest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTreasureChest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTreasureChest::Free()
{
	__super::Free();

	Safe_Release(m_pActivateUI);
}
