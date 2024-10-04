#include "UI_UpCompleted_Crucible.h"

#include "GameInstance.h"

CUI_UpCompleted_Crucible::CUI_UpCompleted_Crucible(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_UpCompleted_Crucible::CUI_UpCompleted_Crucible(const CUI_UpCompleted_Crucible& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_UpCompleted_Crucible::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_UpCompleted_Crucible::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = (g_iWinSizeX >> 1);
	m_fY = (g_iWinSizeY >> 1) + 180.f;
	m_fSizeX = 1024.f; // 1024
	m_fSizeY = 128.f;

	Setting_Position();

	return S_OK;
}

void CUI_UpCompleted_Crucible::Priority_Tick(_float fTimeDelta)
{
}

void CUI_UpCompleted_Crucible::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_UpCompleted_Crucible::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FIFTEENTH);
}

HRESULT CUI_UpCompleted_Crucible::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo27"), TEXT("UPGRADE COMPLETED"), _float2(m_fX - 220.f, m_fY - 25.f), XMVectorSet(0.7f, 0.7f, 0.7f, 1.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpCompleted_Crucible::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Upgrade_Crucible"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpCompleted_Crucible::Bind_ShaderResources()
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

CUI_UpCompleted_Crucible* CUI_UpCompleted_Crucible::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_UpCompleted_Crucible* pInstance = new CUI_UpCompleted_Crucible(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_UpCompleted_Crucible");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_UpCompleted_Crucible::Clone(void* pArg)
{
	CUI_UpCompleted_Crucible* pInstance = new CUI_UpCompleted_Crucible(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_UpCompleted_Crucible");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_UpCompleted_Crucible::Free()
{
	__super::Free();
}
