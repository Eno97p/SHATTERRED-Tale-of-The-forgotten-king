#include "UI_BossHP.h"

#include "GameInstance.h"
#include "Monster.h"

CUI_BossHP::CUI_BossHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_BossHP::CUI_BossHP(const CUI_BossHP& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_BossHP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_BossHP::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = (g_iWinSizeX >> 1) + 46.f;
	m_fY = g_iWinSizeY - 92.f;
	m_fSizeX = 1265.3f; // 2048
	m_fSizeY = 16.f; // 128

	Setting_Position();

	return S_OK;
}

void CUI_BossHP::Priority_Tick(_float fTimeDelta)
{
}

void CUI_BossHP::Tick(_float fTimeDelta)
{
	// 기존의 피보다 늘어났을 때에도 적용이 되어야 하는데 그게 안 되어 있음!
	if (m_fCurrentRatio < m_fPastRatio)
	{
		m_fPastRatio -= fTimeDelta * 0.2f;
		if (m_fCurrentRatio > m_fPastRatio)
		{
			m_fPastRatio = m_fCurrentRatio;
		}
	}
	else if (m_fCurrentRatio > m_fPastRatio)
	{
		m_fPastRatio += fTimeDelta * 0.2f;
		if (m_fCurrentRatio < m_fPastRatio)
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

void CUI_BossHP::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SECOND);
}

HRESULT CUI_BossHP::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue(("g_CurrentRatio"), &m_fCurrentRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue(("g_PastRatio"), &m_fPastRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue(("g_HudRatio"), &m_fHudRatio, sizeof(_float))))
		return E_FAIL;

	m_pShaderCom->Begin(2);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if (m_isDamageRend)
	{
		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo25"), m_wstrDamage, _float2(m_fX + 270.f, m_fY - 50.f), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo23"), m_wstrDamage, _float2(m_fX + 270.f, m_fY - 50.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
			return E_FAIL;
	}

	return S_OK;
}

void CUI_BossHP::Rend_Damage(_int iValue)
{
	_int iDamage = -iValue;
	m_iAccumDamage += iDamage;
	m_isDamageRend = true;
	m_fDamageTimer = 0.f;
	m_wstrDamage = to_wstring(m_iAccumDamage);
}

HRESULT CUI_BossHP::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BossHP"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_BossHP::Bind_ShaderResources()
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

CUI_BossHP* CUI_BossHP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_BossHP* pInstance = new CUI_BossHP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_BossHP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_BossHP::Clone(void* pArg)
{
	CUI_BossHP* pInstance = new CUI_BossHP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_BossHP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_BossHP::Free()
{
	__super::Free();

	Safe_Release(m_pMonster); // nullptr?
}
