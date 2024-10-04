#include "UI_MenuPageTop.h"

#include "GameInstance.h"

CUI_MenuPageTop::CUI_MenuPageTop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_MenuPageTop::CUI_MenuPageTop(const CUI_MenuPageTop& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_MenuPageTop::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_MenuPageTop::Initialize(void* pArg)
{
	UI_TOP_DESC* pDesc = static_cast<UI_TOP_DESC*>(pArg);

	m_eTopType = pDesc->eTopType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = 280.f;
	m_fY = 100.f;
	m_fSizeX = 433.3f; // 650
	m_fSizeY = 147.3f; // 221

	Setting_Position();

	return S_OK;
}

void CUI_MenuPageTop::Priority_Tick(_float fTimeDelta)
{
}

void CUI_MenuPageTop::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_MenuPageTop::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, NINETH);
}

HRESULT CUI_MenuPageTop::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	Render_MenuTitle();

	return S_OK;
}

HRESULT CUI_MenuPageTop::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuickTop"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MenuPageTop::Bind_ShaderResources()
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

void CUI_MenuPageTop::Render_MenuTitle()
{
	// Menu에 따라 제목 출력하기 (Weapon은 제외)
	if (TOP_INV == m_eTopType)
	{
		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), TEXT("INVENTORY"), _float2(m_fX - 43.f, m_fY - 37.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
			return;
	}
}

CUI_MenuPageTop* CUI_MenuPageTop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MenuPageTop* pInstance = new CUI_MenuPageTop(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_MenuPageTop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_MenuPageTop::Clone(void* pArg)
{
	CUI_MenuPageTop* pInstance = new CUI_MenuPageTop(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_MenuPageTop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MenuPageTop::Free()
{
	__super::Free();
}
