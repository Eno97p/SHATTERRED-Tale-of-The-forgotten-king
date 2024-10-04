#include "UI_Shop_RemainIcon.h"

#include "GameInstance.h"

CUI_Shop_RemainIcon::CUI_Shop_RemainIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_Shop_RemainIcon::CUI_Shop_RemainIcon(const CUI_Shop_RemainIcon& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_Shop_RemainIcon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Shop_RemainIcon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Position();

	return S_OK;
}

void CUI_Shop_RemainIcon::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Shop_RemainIcon::Tick(_float fTimeDelta)
{
    if (!m_isRenderAnimFinished)
        Render_Animation(fTimeDelta);
}

void CUI_Shop_RemainIcon::Late_Tick(_float fTimeDelta)
{
    CGameInstance::GetInstance()->Add_UI(this, EIGHT);
}

HRESULT CUI_Shop_RemainIcon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Shop_RemainIcon::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_RemainIcon"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Shop_RemainIcon::Bind_ShaderResources()
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

CUI_Shop_RemainIcon* CUI_Shop_RemainIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Shop_RemainIcon* pInstance = new CUI_Shop_RemainIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Shop_RemainIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Shop_RemainIcon::Clone(void* pArg)
{
	CUI_Shop_RemainIcon* pInstance = new CUI_Shop_RemainIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Shop_RemainIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Shop_RemainIcon::Free()
{
	__super::Free();
}
