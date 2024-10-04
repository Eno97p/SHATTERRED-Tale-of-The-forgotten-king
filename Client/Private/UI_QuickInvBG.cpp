#include "UI_QuickInvBG.h"

#include "GameInstance.h"

CUI_QuickInvBG::CUI_QuickInvBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{pDevice, pContext}
{
}

CUI_QuickInvBG::CUI_QuickInvBG(const CUI_QuickInvBG& rhs)
    : CUI{rhs}
{
}

HRESULT CUI_QuickInvBG::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_QuickInvBG::Initialize(void* pArg)
{
	UI_INVBG_DESC* pDesc = static_cast<UI_INVBG_DESC*>(pArg);

	m_isInv = pDesc->isInv;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Data();
	Setting_Position();

	return S_OK;
}

void CUI_QuickInvBG::Priority_Tick(_float fTimeDelta)
{
}

void CUI_QuickInvBG::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_QuickInvBG::Late_Tick(_float fTimeDelta)
{
	if(m_isInv)
		CGameInstance::GetInstance()->Add_UI(this, NINETH);
	else
		CGameInstance::GetInstance()->Add_UI(this, FIFTH);
}

HRESULT CUI_QuickInvBG::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	// Title / 내용 출력하기


	return S_OK;
}

HRESULT CUI_QuickInvBG::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuickInvBG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_QuickInvBG::Bind_ShaderResources()
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

void CUI_QuickInvBG::Setting_Data()
{
	if (m_isInv)
	{
		m_fX = (g_iWinSizeX >> 1) + 260.f;
		m_fY = (g_iWinSizeY >> 1) - 80.f;
		m_fSizeX = 768.f; // 1024
		m_fSizeY = 768.f;
	}
	else
	{
		m_fX = (g_iWinSizeX >> 1) + 300.f;
		m_fY = (g_iWinSizeY >> 1) - 60.f;
		m_fSizeX = 740.f; // 1024 >> 682
		m_fSizeY = 740.f;
	}
}

CUI_QuickInvBG* CUI_QuickInvBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_QuickInvBG* pInstance = new CUI_QuickInvBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_QuickInvBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_QuickInvBG::Clone(void* pArg)
{
	CUI_QuickInvBG* pInstance = new CUI_QuickInvBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_QuickInvBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_QuickInvBG::Free()
{
	__super::Free();
}
