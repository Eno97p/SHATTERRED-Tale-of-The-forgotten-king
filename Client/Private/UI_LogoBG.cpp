#include "UI_LogoBG.h"

#include "GameInstance.h"

CUI_LogoBG::CUI_LogoBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_LogoBG::CUI_LogoBG(const CUI_LogoBG& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_LogoBG::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_LogoBG::Initialize(void* pArg)
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

void CUI_LogoBG::Priority_Tick(_float fTimeDelta)
{
}

void CUI_LogoBG::Tick(_float fTimeDelta)
{
	m_fGlitchTimer += fTimeDelta;

	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta); // 얘는 수치값등등 다르게 처리?
}

void CUI_LogoBG::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SECOND);
}

HRESULT CUI_LogoBG::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3); // 3
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_LogoBG::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LogoBG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LogoBG::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;



	/*if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_GlitchTimer", &m_fGlitchTimer, sizeof(_float))))
		return E_FAIL;*/






	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CUI_LogoBG* CUI_LogoBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_LogoBG* pInstance = new CUI_LogoBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_LogoBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_LogoBG::Clone(void* pArg)
{
	CUI_LogoBG* pInstance = new CUI_LogoBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_LogoBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_LogoBG::Free()
{
	__super::Free();
}
