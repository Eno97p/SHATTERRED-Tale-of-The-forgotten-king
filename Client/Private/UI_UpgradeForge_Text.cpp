#include "UI_UpgradeForge_Text.h"

#include "GameInstance.h"

CUI_UpgradeForge_Text::CUI_UpgradeForge_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_UpgradeForge_Text::CUI_UpgradeForge_Text(const CUI_UpgradeForge_Text& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_UpgradeForge_Text::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_UpgradeForge_Text::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;
	m_fSizeX = 512.f;
	m_fSizeY = 128.f;

	Setting_Position();

	return S_OK;
}

void CUI_UpgradeForge_Text::Priority_Tick(_float fTimeDelta)
{
}

void CUI_UpgradeForge_Text::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_UpgradeForge_Text::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SIXTH);
}

HRESULT CUI_UpgradeForge_Text::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo27"), TEXT("FORGE"), _float2(m_fX - 50.f, m_fY - 25.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpgradeForge_Text::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Upgrade_Forge_Text"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpgradeForge_Text::Bind_ShaderResources()
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

CUI_UpgradeForge_Text* CUI_UpgradeForge_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_UpgradeForge_Text* pInstance = new CUI_UpgradeForge_Text(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_UpgradeForge_Text");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_UpgradeForge_Text::Clone(void* pArg)
{
	CUI_UpgradeForge_Text* pInstance = new CUI_UpgradeForge_Text(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_UpgradeForge_Text");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_UpgradeForge_Text::Free()
{
	__super::Free();
}
