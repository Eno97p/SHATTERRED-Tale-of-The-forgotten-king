#include "UI_DropItemBG.h"

#include "GameInstance.h"
#include "Inventory.h"

CUI_DropItemBG::CUI_DropItemBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_DropItemBG::CUI_DropItemBG(const CUI_DropItemBG& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_DropItemBG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_DropItemBG::Initialize(void* pArg)
{
	// 여길 들어오기 전에 터지네용??
	UI_DROPITEM_DESC* pDesc = static_cast<UI_DROPITEM_DESC*>(pArg);

	if (pDesc->isItem)
	{
		m_wstrItemName = CInventory::GetInstance()->Get_ItemData(CInventory::GetInstance()->Get_vecItemSize() - 1)->Get_ItemNameText();
	}
	else
	{
		if (pDesc->isWeapon)
		{
			vector<CItemData*>::iterator weapon = CInventory::GetInstance()->Get_Weapons()->begin();
			for (size_t i = 0; i < CInventory::GetInstance()->Get_WeaponSize()-1; ++i)
				++weapon;
			m_wstrItemName = (*weapon)->Get_ItemNameText();
		}
		else
		{
			vector<CItemData*>::iterator skill = CInventory::GetInstance()->Get_Skills()->begin();
			for (size_t i = 0; i < CInventory::GetInstance()->Get_SkillSize() - 1; ++i)
				++skill;
			m_wstrItemName = (*skill)->Get_ItemNameText();
		}
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = 250.f;
	m_fY = (g_iWinSizeY >> 1) + 70.f;
	m_fSizeX = 682.6f; // 1024
	m_fSizeY = 85.3f; // 128

	Setting_Position();

	return S_OK;
}

void CUI_DropItemBG::Priority_Tick(_float fTimeDelta)
{
}

void CUI_DropItemBG::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_DropItemBG::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FIRST);
}

HRESULT CUI_DropItemBG::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), m_wstrItemName, _float2(m_fX - 125.f, m_fY - 15.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_DropItemBG::Add_Components()
{
	/* For. Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUITex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DropItemBG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_DropItemBG::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CUI_DropItemBG* CUI_DropItemBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_DropItemBG* pInstance = new CUI_DropItemBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_DropItemBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_DropItemBG::Clone(void* pArg)
{
	CUI_DropItemBG* pInstance = new CUI_DropItemBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_DropItemBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_DropItemBG::Free()
{
	__super::Free();
}
