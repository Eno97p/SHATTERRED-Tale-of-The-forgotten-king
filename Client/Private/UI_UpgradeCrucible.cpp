#include "UI_UpgradeCrucible.h"

#include "GameInstance.h"

CUI_UpgradeCrucible::CUI_UpgradeCrucible(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_UpgradeCrucible::CUI_UpgradeCrucible(const CUI_UpgradeCrucible& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_UpgradeCrucible::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_UpgradeCrucible::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = (g_iWinSizeX >> 1) + 30.f;
	m_fY = g_iWinSizeY - 80.f;
	m_fSizeX = 512.f;
	m_fSizeY = 64.6f;

	Setting_Position();

	return S_OK;
}

void CUI_UpgradeCrucible::Priority_Tick(_float fTimeDelta)
{
}

void CUI_UpgradeCrucible::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_UpgradeCrucible::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SIXTH);
}

HRESULT CUI_UpgradeCrucible::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo13"), TEXT("UPGRADE THE CRUCIBLE"), _float2(m_fX - 130.f, m_fY - 10.f), XMVectorSet(0.7f, 0.7f, 0.7f, 1.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpgradeCrucible::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Upgrade_Crucible"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpgradeCrucible::Bind_ShaderResources()
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

CUI_UpgradeCrucible* CUI_UpgradeCrucible::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_UpgradeCrucible* pInstance = new CUI_UpgradeCrucible(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_UpgradeCrucible");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_UpgradeCrucible::Clone(void* pArg)
{
	CUI_UpgradeCrucible* pInstance = new CUI_UpgradeCrucible(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_UpgradeCrucible");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_UpgradeCrucible::Free()
{
	__super::Free();
}
