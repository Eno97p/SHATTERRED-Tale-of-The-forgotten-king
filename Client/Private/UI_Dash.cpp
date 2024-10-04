#include "UI_Dash.h"

#include "GameInstance.h"

CUI_Dash::CUI_Dash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_Dash::CUI_Dash(const CUI_Dash& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_Dash::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Dash::Initialize(void* pArg)
{
	UI_DASH_DESC* pDesc = static_cast<UI_DASH_DESC*>(pArg);

	m_iDashIdx = pDesc->iDashIdx;

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

void CUI_Dash::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Dash::Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		if (!m_isRenderAnimFinished)
			Render_Animation(fTimeDelta, 0.8f); // 1.f
		else
			Resset_Animation(!Get_RenderOnAnim());

		m_fAnimTimer += fTimeDelta;
		if (0.6f <= m_fAnimTimer)
		{
			m_fAnimTimer = 0.f;
			m_isAnimUp = !m_isAnimUp;
		}

		if (m_isAnimUp)
		{
			m_fSizeX -= 5.f;
			m_fSizeY -= 5.f;
		}
		else
		{
			m_fSizeX += 5.f;
			m_fSizeY += 5.f;
		}
		Setting_Position();

	}
}

void CUI_Dash::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		CGameInstance::GetInstance()->Add_UI(this, SIXTEENTH);
	}
}

HRESULT CUI_Dash::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(11);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Dash::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dash"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Dash::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iDashIdx)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CUI_Dash* CUI_Dash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Dash* pInstance = new CUI_Dash(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Dash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Dash::Clone(void* pArg)
{
	CUI_Dash* pInstance = new CUI_Dash(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Dash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Dash::Free()
{
	__super::Free();
}
