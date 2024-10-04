#include "stdafx.h"
#include "..\Public\Item.h"

#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"

#include "ItemData.h"
#include "Inventory.h"
#include "UIGroup_DropItem.h"

CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlendObject{ pDevice, pContext }
{
}

CItem::CItem(const CItem& rhs)
	: CBlendObject{ rhs }
{
}

HRESULT CItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{

	


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(0.03f, 0.03f, 0.03f);


	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());

	m_eItemName = static_cast<ITEM_NAME>(rand() % ITEM_END); // 랜덤으로 아이템 종류 설정

	if (nullptr != pArg)
	{
		CItem::ITEM_DESC Desc = *(static_cast<CItem::ITEM_DESC*>(pArg));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(Desc.vPosition.x, Desc.vPosition.y, Desc.vPosition.z, 1.f));
		//m_pTextureTransformCom.set


		//Desc.vPosition.x;
		//Desc.vPosition.y;
		//Desc.vPosition.z;


	}
	Set_Texture();






	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(166.3f, 9.7f, 13.0f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(193.0f, 16.3f, -25.3f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-4.4f, 22.0f, -122.7f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(102.0f, 13.5f, -110.0f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(191.0f, 3.8f, -46.0f, 1.f));
	
	return S_OK;
}

void CItem::Set_Texture()
{
	// m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(145.f, 524.f, 98.f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(rand() % 10 + 145, 524.f, 98.f, 1.f));
	m_pTextureTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
	m_pTextureTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);
	m_pTextureTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(
		XMVectorGetX(m_pTextureTransformCom->Get_State(CTransform::STATE_POSITION)),
		XMVectorGetY(m_pTextureTransformCom->Get_State(CTransform::STATE_POSITION)) + 0.1f,
		XMVectorGetZ(m_pTextureTransformCom->Get_State(CTransform::STATE_POSITION)), 1.f));
}

void CItem::Priority_Tick(_float fTimeDelta)
{
}

void CItem::Tick(_float fTimeDelta)
{
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fTimeDelta);

	m_pTextureTransformCom->BillBoard();

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	if (m_pColliderCom->Intersect(m_pPlayer->Get_Collider()) == CCollider::COLL_START)
	{
 		CInventory::GetInstance()->Add_DropItem(m_eItemName); // m_eItemName   ITEM_BUFF1

		m_pGameInstance->Disable_Echo();
		m_pGameInstance->Play_Effect_Sound(TEXT("GetItem.mp3"), SOUND_EFFECT);

		// 아이템 획득 로직
		m_pGameInstance->Erase(this);
	}
}

void CItem::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DISTORTION, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CItem::Render()
{
	if (FAILED(m_pTextureTransformCom->Bind_ShaderResource(m_pTextureShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pTextureShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pTextureShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pTextureShaderCom, "g_Texture", 0)))
		return E_FAIL;

	m_pTextureShaderCom->Begin(2);

	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();



	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_float4))))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		//m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(10);

		m_pModelCom->Render(i);
	}


	return S_OK;
}

HRESULT CItem::Render_Bloom()
{
	if (FAILED(m_pTextureTransformCom->Bind_ShaderResource(m_pTextureShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pTextureShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pTextureShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pTextureShaderCom, "g_Texture", 0)))
		return E_FAIL;

	m_pTextureShaderCom->Begin(3);

	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();
}

HRESULT CItem::Render_Distortion()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		//m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(3);

		m_pModelCom->Render(i);
	}


	return S_OK;
}

HRESULT CItem::Add_Components()
{
	/* For. Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Item"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_TextureShader"), reinterpret_cast<CComponent**>(&m_pTextureShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	m_pTextureTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTextureTransformCom)
		return E_FAIL;
	m_pTextureTransformCom->Initialize(nullptr);

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(7.f, 7.f, 7.f);
	ColliderDesc.vCenter = _float3(0.f, 5.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

CItem* CItem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem* pInstance = new CItem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem::Clone(void* pArg)
{
	CItem* pInstance = new CItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem::Free()
{
	__super::Free();

	m_pPlayer = nullptr;

	Safe_Release(m_pTextureShaderCom);
	Safe_Release(m_pTextureTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
}
