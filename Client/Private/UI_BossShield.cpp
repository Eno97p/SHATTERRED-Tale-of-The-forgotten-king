#include "UI_BossShield.h"

#include "GameInstance.h"
#include "Monster.h"

CUI_BossShield::CUI_BossShield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_BossShield::CUI_BossShield(const CUI_BossShield& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_BossShield::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_BossShield::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	
	// ¹ÝÂÉ°¡¸®
	//m_fX = (g_iWinSizeX >> 1) + 207.f; // 46.f
	//m_fY = g_iWinSizeY - 92.f;
	//m_fSizeX = 632.65f; // 2048    1265.3f
	//m_fSizeY = 16.f; // 128

	m_fX = (g_iWinSizeX >> 1) + 46.f; // 46.f
	m_fY = g_iWinSizeY - 92.f;
	m_fSizeX = 1265.3f; // 2048    1265.3f
	m_fSizeY = 16.f; // 128

	Setting_Position();

	return S_OK;
}

void CUI_BossShield::Priority_Tick(_float fTimeDelta)
{
}

void CUI_BossShield::Tick(_float fTimeDelta)
{
	if (m_fCurrentRatio < m_fPastRatio)
	{
		m_fPastRatio -= fTimeDelta * 0.2f;
		if (m_fCurrentRatio > m_fPastRatio)
		{
			m_fPastRatio = m_fCurrentRatio;
		}
	}

	if (m_isDamageRend)
	{
		m_fDamageTimer += fTimeDelta;
		if (2. <= m_fDamageTimer)
		{
			m_iAccumDamage = 0;
			m_isDamageRend = false;
		}
	}
}

void CUI_BossShield::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, THIRD);
}

HRESULT CUI_BossShield::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_BossShield::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BossShield"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_BossShield::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue(("g_CurrentRatio"), &m_fCurrentRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue(("g_PastRatio"), &m_fPastRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue(("g_HudRatio"), &m_fHudRatio, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUI_BossShield* CUI_BossShield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_BossShield* pInstance = new CUI_BossShield(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_BossShield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_BossShield::Clone(void* pArg)
{
	CUI_BossShield* pInstance = new CUI_BossShield(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_BossShield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_BossShield::Free()
{
	__super::Free();

	Safe_Release(m_pMonster); // nullptr?
}
