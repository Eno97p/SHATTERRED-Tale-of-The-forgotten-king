#include "UI_StateHP.h"

#include "GameInstance.h"
#include "Player.h"

CUI_StateHP::CUI_StateHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{pDevice, pContext}
{
}

CUI_StateHP::CUI_StateHP(const CUI_StateHP& rhs)
    : CUI{rhs}
{
}

HRESULT CUI_StateHP::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_StateHP::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = 282.2f; // 390
	m_fY = 45.f;
	m_fSizeX = 300.f; // 768
	m_fSizeY = 24.f;

	Setting_Position();

	return S_OK;
}

void CUI_StateHP::Priority_Tick(_float fTimeDelta)
{
	
}

void CUI_StateHP::Tick(_float fTimeDelta)
{
	if (!m_pPlayer)
	{
		list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
		m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
		Safe_AddRef(m_pPlayer);
	}
	else
	{
		m_fCurrentRatio = m_pPlayer->Get_HpRatio();
	}

	m_fSizeX = m_pPlayer->Get_MaxHP(); // 134   500
	m_fX = ORIGIN_X - ((ORIGIN_SIZEX - m_fSizeX) / 2 * 0.77f); // 282.8f
	Setting_Position();

	// 체력 감소
	if (m_fCurrentRatio < m_fPastRatio)
	{
		m_fPastRatio -= fTimeDelta * 0.2f;
		if (m_fCurrentRatio > m_fPastRatio)
		{
			m_fPastRatio = m_fCurrentRatio;
		}
	}
	// 체력 증가
	else if (m_fCurrentRatio > m_fPastRatio)
	{
		m_fPastRatio += fTimeDelta * 0.2f;
		if (m_fCurrentRatio < m_fPastRatio)
		{
			m_fPastRatio = m_fCurrentRatio;
		}
	}
}

void CUI_StateHP::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, THIRD);
}

HRESULT CUI_StateHP::Render()
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

	return S_OK;
}

void CUI_StateHP::Resset_Player()
{
	Safe_Release(m_pPlayer);
	m_pPlayer = nullptr;
}

HRESULT CUI_StateHP::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateHP"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_StateHP::Bind_ShaderResources()
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

CUI_StateHP* CUI_StateHP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_StateHP* pInstance = new CUI_StateHP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_StateHP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_StateHP::Clone(void* pArg)
{
	CUI_StateHP* pInstance = new CUI_StateHP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_StateHP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_StateHP::Free()
{
	__super::Free();
	Safe_Release(m_pPlayer);
}
