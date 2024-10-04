#include "UI_StateBar.h"

#include "GameInstance.h"
#include "Player.h"

CUI_StateBar::CUI_StateBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{pDevice, pContext}
{
}

CUI_StateBar::CUI_StateBar(const CUI_StateBar& rhs)
    : CUI{rhs}
{
}

HRESULT CUI_StateBar::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_StateBar::Initialize(void* pArg)
{
	UI_STATEBAR_DESC* pDesc = static_cast<UI_STATEBAR_DESC*>(pArg);

	m_eBarType = pDesc->eBarType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_XY();
	m_fSizeX = 768.f;
	m_fSizeY = 24.f;

	Setting_Position();

	return S_OK;
}

void CUI_StateBar::Priority_Tick(_float fTimeDelta)
{
}

void CUI_StateBar::Tick(_float fTimeDelta)
{

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	CPlayer* pPlayer = nullptr;
	if (!PlayerList.empty())
	{
		pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());

		switch (m_eBarType)
		{
		case Client::CUI_StateBar::BAR_HP:
			m_fSizeX = pPlayer->Get_MaxHP(); // 134   500
			break;
		case Client::CUI_StateBar::BAR_ENERGY:
			m_fSizeX = pPlayer->Get_MaxStamina(); // 134   500
			break;
		case Client::CUI_StateBar::BAR_ETHER:
			m_fSizeX = pPlayer->Get_MaxMP(); // 134   500
			break;
		default:
			break;
		}

		m_fX = ORIGIN_X - ((ORIGIN_SIZEX - m_fSizeX) / 2 * 0.77f); // 282.8f
		Setting_Position();
	}


}

void CUI_StateBar::Late_Tick(_float fTimeDelta)
{
    CGameInstance::GetInstance()->Add_UI(this, SECOND);
}

HRESULT CUI_StateBar::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_StateBar::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateBar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_StateBar::Bind_ShaderResources()
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

void CUI_StateBar::Setting_XY()
{
	switch (m_eBarType)
	{
	case Client::CUI_StateBar::BAR_HP:
		m_fX = 390.f;
		m_fY = 45;
		break;
	case Client::CUI_StateBar::BAR_ENERGY:
		m_fX = 390.f;
		m_fY = 60.f;
		break;
	case Client::CUI_StateBar::BAR_ETHER:
		m_fX = 390.f;
		m_fY = 75;
		break;
	default:
		break;
	}
}

CUI_StateBar* CUI_StateBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_StateBar* pInstance = new CUI_StateBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_StateBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_StateBar::Clone(void* pArg)
{
	CUI_StateBar* pInstance = new CUI_StateBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_StateBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_StateBar::Free()
{
	__super::Free();
}
