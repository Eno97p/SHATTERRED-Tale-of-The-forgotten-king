#include "UI_Setting_Star.h"

#include "GameInstance.h"

CUI_Setting_Star::CUI_Setting_Star(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Setting_Star::CUI_Setting_Star(const CUI_Setting_Star& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Setting_Star::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Setting_Star::Initialize(void* pArg)
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

void CUI_Setting_Star::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Setting_Star::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	m_fFlowTimer -= fTimeDelta * 0.03f;
	if (-10.f >= m_fFlowTimer)
	{
		m_fFlowTimer = 0.f;
	}
}

void CUI_Setting_Star::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, NINETH);
}

HRESULT CUI_Setting_Star::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(9); // 6 아니고 세로로 흐르는 것으로
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Setting_Star::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Setting_Star"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Setting_Star::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFlowTime", &m_fFlowTimer, sizeof(_float))))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
	//	return E_FAIL;

	return S_OK;
}

CUI_Setting_Star* CUI_Setting_Star::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Setting_Star* pInstance = new CUI_Setting_Star(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Setting_Star");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Setting_Star::Clone(void* pArg)
{
	CUI_Setting_Star* pInstance = new CUI_Setting_Star(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Setting_Star");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Setting_Star::Free()
{
	__super::Free();
}
