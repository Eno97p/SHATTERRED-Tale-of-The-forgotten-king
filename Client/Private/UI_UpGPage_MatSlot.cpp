#include "UI_UpGPage_MatSlot.h"

#include "GameInstance.h"
#include "UI_ItemIcon.h"

CUI_UpGPage_MatSlot::CUI_UpGPage_MatSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_UpGPage_MatSlot::CUI_UpGPage_MatSlot(const CUI_UpGPage_MatSlot& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_UpGPage_MatSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_UpGPage_MatSlot::Initialize(void* pArg)
{
	UI_MATSLOT_DESC* pDesc = static_cast<UI_MATSLOT_DESC*>(pArg);

	m_eMatSlotType = pDesc->eMatSlotType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (MATSLOT_L == m_eMatSlotType)
	{
		m_fX = (g_iWinSizeX >> 1) - 65.f;
		m_fY = (g_iWinSizeY >> 1) + 100.f;
	}
	else if (MATSLOT_R == m_eMatSlotType)
	{
		m_fX = (g_iWinSizeX >> 1) + 30.f;
		m_fY = (g_iWinSizeY >> 1) + 100.f;
	}

	m_fSizeX = 85.3f; // 128
	m_fSizeY = 85.3f;

	Setting_Position();

	// Mat Slot Type에 따라 ItemIcon 다른 것 출력해두어야 함
	Create_ItemIcon();

	return S_OK;
}

void CUI_UpGPage_MatSlot::Priority_Tick(_float fTimeDelta)
{
}

void CUI_UpGPage_MatSlot::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	if (nullptr != m_pItemIcon)
		m_pItemIcon->Tick(fTimeDelta);
}

void CUI_UpGPage_MatSlot::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, TENTH);

	if (nullptr != m_pItemIcon)
		m_pItemIcon->Late_Tick(fTimeDelta);
}

HRESULT CUI_UpGPage_MatSlot::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_UpGPage_MatSlot::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpGPage_MatSlot"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpGPage_MatSlot::Bind_ShaderResources()
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

HRESULT CUI_UpGPage_MatSlot::Create_ItemIcon()
{
	//m_pItemIcon

	CUI_ItemIcon::UI_ITEMICON_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 64.f;
	pDesc.fSizeY = 64.f;
	pDesc.eUISort = ELEVENTH;

	if (MATSLOT_L == m_eMatSlotType)
	{
		pDesc.wszTexture = TEXT("Prototype_Component_Texture_HUD_StateSoul");
	}
	else if (MATSLOT_R == m_eMatSlotType)
	{
		pDesc.wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Upgrade0"); // 심화 강화가 필요한 Weapon의 경우 Upgrade1 텍스쳐가 필요할 것임 (일단 임의로)
	}

	m_pItemIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pDesc));

	return S_OK;
}

CUI_UpGPage_MatSlot* CUI_UpGPage_MatSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_UpGPage_MatSlot* pInstance = new CUI_UpGPage_MatSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_UpGPage_MatSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_UpGPage_MatSlot::Clone(void* pArg)
{
	CUI_UpGPage_MatSlot* pInstance = new CUI_UpGPage_MatSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_UpGPage_MatSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_UpGPage_MatSlot::Free()
{
	__super::Free();

	Safe_Release(m_pItemIcon);
}
