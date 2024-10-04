#include "UI_BossHPBar.h"

#include "GameInstance.h"

CUI_BossHPBar::CUI_BossHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_BossHPBar::CUI_BossHPBar(const CUI_BossHPBar& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_BossHPBar::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_BossHPBar::Initialize(void* pArg)
{
	UI_BOSSHPBAR_DESC* pDesc = static_cast<UI_BOSSHPBAR_DESC*>(pArg);

	wcscpy_s(m_wszBossName, pDesc->wszBossName);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = (g_iWinSizeX >> 1) + 50.f;
	m_fY = g_iWinSizeY - 100.f;
	m_fSizeX = 1265.3f; // 2048
	m_fSizeY = 128.f; // 128

	Setting_Position();

	return S_OK;
}

void CUI_BossHPBar::Priority_Tick(_float fTimeDelta)
{
}

void CUI_BossHPBar::Tick(_float fTimeDelta)
{
}

void CUI_BossHPBar::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FIRST);
}

HRESULT CUI_BossHPBar::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	// Render m_wszBossName
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo17"), m_wszBossName, _float2(m_fX - 330.f, m_fY - 30.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_BossHPBar::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BossHPBar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_BossHPBar::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CUI_BossHPBar* CUI_BossHPBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_BossHPBar* pInstance = new CUI_BossHPBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_BossHPBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_BossHPBar::Clone(void* pArg)
{
	CUI_BossHPBar* pInstance = new CUI_BossHPBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_BossHPBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_BossHPBar::Free()
{
	__super::Free();
}
