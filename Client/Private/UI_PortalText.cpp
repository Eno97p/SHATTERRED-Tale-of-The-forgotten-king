#include "UI_PortalText.h"

#include "GameInstance.h"

#include "Player.h"

CUI_PortalText::CUI_PortalText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_PortalText::CUI_PortalText(const CUI_PortalText& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_PortalText::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_PortalText::Initialize(void* pArg)
{
	UI_PORTALTEXT_DESC* pDesc = static_cast<UI_PORTALTEXT_DESC*>(pArg);

	m_vTargetPos = pDesc->vPos;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY - 150.f;
	m_fSizeX = 512.f;
	m_fSizeY = 512.f;
	m_fDistance = pDesc->fDistance;

	Setting_Position();

	Setting_Text(pDesc->ePortalLevel);

	m_isRend = true;

	return S_OK;
}

void CUI_PortalText::Priority_Tick(_float fTimeDelta)
{
}

void CUI_PortalText::Tick(_float fTimeDelta)
{
	Check_Distance();
}

void CUI_PortalText::Late_Tick(_float fTimeDelta)
{
	if (m_isRend) {
		CGameInstance::GetInstance()->Add_UI(this, FIRST);
		
	}
}

HRESULT CUI_PortalText::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), m_fText, _float2(m_fX - m_fFontX, m_fY - 10.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), m_fText, _float2(m_fX - m_fFontX, m_fY - 10.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PortalText::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PortalText"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PortalText::Bind_ShaderResources()
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

void CUI_PortalText::Setting_Text(LEVEL ePortalLevel)
{
	switch (ePortalLevel)
	{
	case Client::LEVEL_ACKBAR:
		m_fFontX = 50.f;
		m_fText = TEXT("Go To Ackbar");
		break;
	case Client::LEVEL_JUGGLAS:
		m_fFontX = 100.f;
		m_fText = TEXT("Go To Shamra's Grotto");
		break;
	case Client::LEVEL_GRASSLAND:
		m_fFontX = 80.f;
		m_fText = TEXT("Go To Limbo Plains");
		break;
	case Client::LEVEL_ANDRASARENA:
		m_fFontX = 110.f;
		m_fText = TEXT("Go To King's Whilderness");
		break;
	default:
		m_fText = TEXT("");
		break;
	}
}

void CUI_PortalText::Check_Distance()
{
	// Player와의 거리 계산

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));

	if (PlayerList.empty())
		return;

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(dynamic_cast<CPlayer*>(PlayerList.front())->Get_Component(TEXT("Com_Transform")));
	
	_vector vBetween = pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_vTargetPos;
	_float fDistance = XMVectorGetX(XMVector4Length(vBetween));

	if (m_fDistance >= fDistance)
	{
		m_isRend = true;
	}
	else
	{
		m_isRend = false;
	}
}

CUI_PortalText* CUI_PortalText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_PortalText* pInstance = new CUI_PortalText(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_PortalText");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_PortalText::Clone(void* pArg)
{
	CUI_PortalText* pInstance = new CUI_PortalText(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_PortalText");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PortalText::Free()
{
	__super::Free();
}
