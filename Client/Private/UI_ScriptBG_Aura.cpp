#include "UI_ScriptBG_Aura.h"

#include "GameInstance.h"

CUI_ScriptBG_Aura::CUI_ScriptBG_Aura(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_ScriptBG_Aura::CUI_ScriptBG_Aura(const CUI_ScriptBG_Aura& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_ScriptBG_Aura::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_ScriptBG_Aura::Initialize(void* pArg)
{
	UI_AURA_DESC* pDesc = static_cast<UI_AURA_DESC*>(pArg);

	m_iTextureNum = pDesc->iTextureNum;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;
	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	Setting_Position();

	return S_OK;
}

void CUI_ScriptBG_Aura::Priority_Tick(_float fTimeDelta)
{
}

void CUI_ScriptBG_Aura::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_ScriptBG_Aura::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FOURTH); // HUD, Weapon Slot보다 위에 덮이도록
}

HRESULT CUI_ScriptBG_Aura::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(8);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_ScriptBG_Aura::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Script_Aura"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ScriptBG_Aura::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CUI_ScriptBG_Aura* CUI_ScriptBG_Aura::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_ScriptBG_Aura* pInstance = new CUI_ScriptBG_Aura(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_ScriptBG_Aura");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_ScriptBG_Aura::Clone(void* pArg)
{
	CUI_ScriptBG_Aura* pInstance = new CUI_ScriptBG_Aura(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_ScriptBG_Aura");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_ScriptBG_Aura::Free()
{
	__super::Free();
}
