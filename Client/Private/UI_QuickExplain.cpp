#include "UI_QuickExplain.h"

#include "GameInstance.h"

CUI_QuickExplain::CUI_QuickExplain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_QuickExplain::CUI_QuickExplain(const CUI_QuickExplain& rhs)
	: CUI{rhs}
{
}

HRESULT CUI_QuickExplain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_QuickExplain::Initialize(void* pArg)
{
	UI_EXPLAIN_DESC* pDesc = static_cast<UI_EXPLAIN_DESC*>(pArg);

	m_eUISort = pDesc->eUISort;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = (g_iWinSizeX >> 1) + 10.f;
	m_fY = g_iWinSizeY - 150.f;
	m_fSizeX = 1365.3f; // 2048
	m_fSizeY = 341.3f; // 512

	Setting_Position();

	return S_OK;
}

void CUI_QuickExplain::Priority_Tick(_float fTimeDelta)
{
}

void CUI_QuickExplain::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_QuickExplain::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, m_eUISort);
}

HRESULT CUI_QuickExplain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_QuickExplain::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuickExplain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_QuickExplain::Bind_ShaderResources()
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

CUI_QuickExplain* CUI_QuickExplain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_QuickExplain* pInstance = new CUI_QuickExplain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_QuickExplain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_QuickExplain::Clone(void* pArg)
{
	CUI_QuickExplain* pInstance = new CUI_QuickExplain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_QuickExplain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_QuickExplain::Free()
{
	__super::Free();
}
