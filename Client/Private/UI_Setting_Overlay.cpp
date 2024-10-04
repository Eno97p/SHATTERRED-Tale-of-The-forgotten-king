#include "UI_Setting_Overlay.h"

#include "GameInstance.h"

CUI_Setting_Overlay::CUI_Setting_Overlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Setting_Overlay::CUI_Setting_Overlay(const CUI_Setting_Overlay& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Setting_Overlay::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Setting_Overlay::Initialize(void* pArg)
{
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

void CUI_Setting_Overlay::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Setting_Overlay::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_Setting_Overlay::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, TENTH);
}

HRESULT CUI_Setting_Overlay::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(10);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo23"), TEXT("GAME OPTIONS"), _float2((g_iWinSizeX >> 1) - 130.f, 90.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Setting_Overlay::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Setting_Overlay"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Setting_Overlay::Bind_ShaderResources()
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

CUI_Setting_Overlay* CUI_Setting_Overlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Setting_Overlay* pInstance = new CUI_Setting_Overlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Setting_Overlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Setting_Overlay::Clone(void* pArg)
{
	CUI_Setting_Overlay* pInstance = new CUI_Setting_Overlay(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Setting_Overlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Setting_Overlay::Free()
{
	__super::Free();
}
