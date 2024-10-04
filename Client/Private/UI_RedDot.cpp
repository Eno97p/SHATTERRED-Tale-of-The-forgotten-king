#include "UI_RedDot.h"

#include "GameInstance.h"

CUI_RedDot::CUI_RedDot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_RedDot::CUI_RedDot(const CUI_RedDot& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_RedDot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_RedDot::Initialize(void* pArg)
{
	UI_REDDOT_DESC* pDesc = static_cast<UI_REDDOT_DESC*>(pArg);

	m_eUISort = pDesc->eUISort;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Position();

	return S_OK;
}

void CUI_RedDot::Priority_Tick(_float fTimeDelta)
{
}

void CUI_RedDot::Tick(_float fTimeDelta)
{
	// 서서히 나타날 필요 없을 거 같은데 >> 고쳐주기
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_RedDot::Late_Tick(_float fTimeDelta)
{
	if(m_isRend)
		CGameInstance::GetInstance()->Add_UI(this, m_eUISort);
}

HRESULT CUI_RedDot::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_RedDot::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_RedDot"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_RedDot::Bind_ShaderResources()
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

CUI_RedDot* CUI_RedDot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_RedDot* pInstance = new CUI_RedDot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_RedDot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_RedDot::Clone(void* pArg)
{
	CUI_RedDot* pInstance = new CUI_RedDot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_RedDot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_RedDot::Free()
{
	__super::Free();
}
