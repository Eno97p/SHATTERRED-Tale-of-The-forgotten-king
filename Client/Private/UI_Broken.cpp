#include "UI_Broken.h"

#include "GameInstance.h"

CUI_Broken::CUI_Broken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Broken::CUI_Broken(const CUI_Broken& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Broken::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Broken::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;
	m_fSizeX = g_iWinSizeX * 0.2f; // * 0.2f
	m_fSizeY = g_iWinSizeY * 0.2f;

	Setting_Position();

	return S_OK;
}

void CUI_Broken::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Broken::Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		m_fDeadTimer += fTimeDelta;

		m_fSizeX *= 1.09f;
		m_fSizeY *= 1.1f;
		Setting_Position();
	}

	if (0.5f <= m_fDeadTimer)
	{
		m_isRend = false;
		m_fDeadTimer = 0.f;
		m_fSizeX = g_iWinSizeX * 0.1f;
		m_fSizeY = g_iWinSizeY * 0.1f;
	}
	else
	{
		if (!m_isRenderAnimFinished)
			Render_Animation(fTimeDelta, 1.f);
	}
}

void CUI_Broken::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		CGameInstance::GetInstance()->Add_UI(this, SEVENTEENTH);
	}
}

HRESULT CUI_Broken::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Broken::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Broken"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Broken::Bind_ShaderResources()
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

CUI_Broken* CUI_Broken::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Broken* pInstance = new CUI_Broken(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Broken");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Broken::Clone(void* pArg)
{
	CUI_Broken* pInstance = new CUI_Broken(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Broken");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Broken::Free()
{
	__super::Free();
}
