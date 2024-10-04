#include "UI_MenuAlphaBG.h"

#include "GameInstance.h"

CUI_MenuAlphaBG::CUI_MenuAlphaBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_MenuAlphaBG::CUI_MenuAlphaBG(const CUI_MenuAlphaBG& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_MenuAlphaBG::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_MenuAlphaBG::Initialize(void* pArg)
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

void CUI_MenuAlphaBG::Priority_Tick(_float fTimeDelta)
{
}

void CUI_MenuAlphaBG::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	m_fFlowTimer += fTimeDelta * 0.03f;
	if (10.f <= m_fFlowTimer)
	{
		m_fFlowTimer = 0.f;
	}
}

void CUI_MenuAlphaBG::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FOURTH);
}

HRESULT CUI_MenuAlphaBG::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(6);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_MenuAlphaBG::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuAlphaBG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MenuAlphaBG::Bind_ShaderResources()
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

CUI_MenuAlphaBG* CUI_MenuAlphaBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MenuAlphaBG* pInstance = new CUI_MenuAlphaBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_MenuAlphaBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_MenuAlphaBG::Clone(void* pArg)
{
	CUI_MenuAlphaBG* pInstance = new CUI_MenuAlphaBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_MenuAlphaBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MenuAlphaBG::Free()
{
	__super::Free();
}
