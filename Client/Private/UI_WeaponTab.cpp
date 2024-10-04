#include "UI_WeaponTab.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "UIGroup_Weapon.h"

CUI_WeaponTab::CUI_WeaponTab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_WeaponTab::CUI_WeaponTab(const CUI_WeaponTab& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_WeaponTab::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_WeaponTab::Initialize(void* pArg)
{
	UI_TAB_DESC* pDesc = static_cast<UI_TAB_DESC*>(pArg);

	m_eTabType = pDesc->eTabType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Data();
	Setting_Position();

	if (TAB_L == m_eTabType)
		m_isActivate = true;

	return S_OK;
}

void CUI_WeaponTab::Priority_Tick(_float fTimeDelta)
{
}

void CUI_WeaponTab::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	Change_Activate();
	Change_Size();
}

void CUI_WeaponTab::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, NINETH);
}

HRESULT CUI_WeaponTab::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	Render_Text();

	return S_OK;
}

HRESULT CUI_WeaponTab::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WeaponTab"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_WeaponTab::Bind_ShaderResources()
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

void CUI_WeaponTab::Setting_Data()
{
	switch (m_eTabType)
	{
	case Client::CUI_WeaponTab::TAB_L:
	{
		m_fX = (g_iWinSizeX >> 1) - 35.f;
		m_fSizeX = SELECT_SIZE; // 128
		m_fSizeY = SELECT_SIZE;
		break;
	}
	case Client::CUI_WeaponTab::TAB_R:
	{
		m_fX = (g_iWinSizeX >> 1) + 35.f;
		m_fSizeX = NONE_SIZE; // 128
		m_fSizeY = NONE_SIZE;
		break;
	}
	default:
		break;
	}

	m_fY = 80.f;
}

_tchar* CUI_WeaponTab::Settiing_BtnText()
{
	switch (m_eTabType)
	{
	case Client::CUI_WeaponTab::TAB_L:
		m_fFontX = m_fX - 350.f;
		return TEXT("WEAPONS");
	case Client::CUI_WeaponTab::TAB_R:
		m_fFontX = m_fX + 170.f;
		return TEXT("ARTEFACTS");
	default:
		return TEXT("");
	}
}

void CUI_WeaponTab::Render_Text()
{
	_tchar wszFont[MAX_PATH] = TEXT("");
	_float fX = { 0.f };

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo13"), TEXT("TAB"), _float2((g_iWinSizeX >> 1) - 17.f, m_fY - 30.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;

	if (m_isActivate)
	{
		wcscpy_s(wszFont, TEXT("Font_Cardo17"));
	}
	else
	{
		wcscpy_s(wszFont, TEXT("Font_Cardo15"));
	}

	if (FAILED(m_pGameInstance->Render_Font(wszFont, Settiing_BtnText(), _float2(m_fFontX, m_fY - 10.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return;
}

void CUI_WeaponTab::Change_Activate()
{
	// Get_TabType
	if (CUIGroup_Weapon::TAB_L == dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Get_TabType())
	{
		if (TAB_L == m_eTabType)
			m_isActivate = true;
		else
			m_isActivate = false;
	}
	else if (CUIGroup_Weapon::TAB_R == dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Get_TabType())
	{
		if (TAB_R == m_eTabType)
			m_isActivate = true;
		else
			m_isActivate = false;
	}
}

void CUI_WeaponTab::Change_Size()
{
	if (m_isActivate)
	{
		m_fSizeX = SELECT_SIZE;
		m_fSizeY = SELECT_SIZE;
	}
	else
	{
		m_fSizeX = NONE_SIZE;
		m_fSizeY = NONE_SIZE;
	}

	Setting_Position();
}

CUI_WeaponTab* CUI_WeaponTab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_WeaponTab* pInstance = new CUI_WeaponTab(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_WeaponTab");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_WeaponTab::Clone(void* pArg)
{
	CUI_WeaponTab* pInstance = new CUI_WeaponTab(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_WeaponTab");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_WeaponTab::Free()
{
	__super::Free();
}
