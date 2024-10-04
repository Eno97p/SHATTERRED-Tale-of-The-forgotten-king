#include "UI_QuickTop.h"

#include "GameInstance.h"

CUI_QuickTop::CUI_QuickTop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{pDevice, pContext}
{
}

CUI_QuickTop::CUI_QuickTop(const CUI_QuickTop& rhs)
	: CUI{rhs}
{
}

HRESULT CUI_QuickTop::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_QuickTop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = 270.f;
	m_fY = 140.f;
	m_fSizeX = 433.3f; // 650
	m_fSizeY = 147.3f; // 221

	Setting_Position();

	return S_OK;
}

void CUI_QuickTop::Priority_Tick(_float fTimeDelta)
{
}

void CUI_QuickTop::Tick(_float fTimeDelta)
{
	if(!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_QuickTop::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FIFTH);
}

HRESULT CUI_QuickTop::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_QuickTop::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuickTop"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_QuickTop::Bind_ShaderResources()
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

CUI_QuickTop* CUI_QuickTop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_QuickTop* pInstance = new CUI_QuickTop(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_QuickTop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_QuickTop::Clone(void* pArg)
{
	CUI_QuickTop* pInstance = new CUI_QuickTop(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_QuickTop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_QuickTop::Free()
{
	__super::Free();
}
