#include "UI_Menu_SelectFrame.h"

#include "GameInstance.h"

CUI_Menu_SelectFrame::CUI_Menu_SelectFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{pDevice, pContext}
{
}

CUI_Menu_SelectFrame::CUI_Menu_SelectFrame(const CUI_Menu_SelectFrame& rhs)
	: CUI{rhs}
{
}

HRESULT CUI_Menu_SelectFrame::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Menu_SelectFrame::Initialize(void* pArg)
{
	UI_MENUSELECTFRAME_DESC* pDesc = static_cast<UI_MENUSELECTFRAME_DESC*>(pArg);

	m_eMenuType = pDesc->eMenuType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_XY();
	m_fSizeX = 168.6f; // 170.6
	m_fSizeY = 168.6f;

	Setting_Position();

	return S_OK;
}

void CUI_Menu_SelectFrame::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Menu_SelectFrame::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	m_fFlowTimer += fTimeDelta * 0.2f;
	if (10.f <= m_fFlowTimer)
	{
		m_fFlowTimer = 0.f;
	}
}

void CUI_Menu_SelectFrame::Late_Tick(_float fTimeDelta)
{
	if(m_isRenderAnimFinished) // 애니메이션 다 돌고 나서 렌더 되도록 하기? 그 전에 보이면 안됨
		CGameInstance::GetInstance()->Add_UI(this, SIXTH);
}

HRESULT CUI_Menu_SelectFrame::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Menu_SelectFrame::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_SelectFrame"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Mask"),
		TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Menu_SelectFrame::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFlowTime", &m_fFlowTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

void CUI_Menu_SelectFrame::Setting_XY()
{
	switch (m_eMenuType)
	{
	case Client::CUI_Menu_SelectFrame::MENU_CH:
		m_fX = g_iWinSizeX >> 1;
		m_fY = 130.f;
		break;
	case Client::CUI_Menu_SelectFrame::MENU_MAP:
		m_fX = (g_iWinSizeX >> 1) - 100.f;
		m_fY = 225.f;
		break;
	case Client::CUI_Menu_SelectFrame::MENU_WEAPON:
		m_fX = (g_iWinSizeX >> 1) + 100.f;
		m_fY = 225.f;
		break;
	case Client::CUI_Menu_SelectFrame::MENU_INV:
		m_fX = g_iWinSizeX >> 1;
		m_fY = 320.f;
		break;
	case Client::CUI_Menu_SelectFrame::MENU_SET:
		m_fX = (g_iWinSizeX >> 1) - 100.f;
		m_fY = 415.f;
		break;
	case Client::CUI_Menu_SelectFrame::MENU_COD:
		m_fX = g_iWinSizeX >> 1;
		m_fY = 510.f;
		break;
	default:
		break;
	}
}

CUI_Menu_SelectFrame* CUI_Menu_SelectFrame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Menu_SelectFrame* pInstance = new CUI_Menu_SelectFrame(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Menu_SelectFrame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Menu_SelectFrame::Clone(void* pArg)
{
	CUI_Menu_SelectFrame* pInstance = new CUI_Menu_SelectFrame(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Menu_SelectFrame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Menu_SelectFrame::Free()
{
	__super::Free();

	Safe_Release(m_pMaskTextureCom);
}
