#include "UI_StateSoul.h"

#include "GameInstance.h"
#include "Inventory.h"
#include "Player.h"

CUI_StateSoul::CUI_StateSoul(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{pDevice, pContext}
{
}

CUI_StateSoul::CUI_StateSoul(const CUI_StateSoul& rhs)
    : CUI{rhs}
{
}

HRESULT CUI_StateSoul::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_StateSoul::Initialize(void* pArg)
{
	UI_SOUL_DESC* pDesc = static_cast<UI_SOUL_DESC*>(pArg);

	m_isSoulCntRend = pDesc->isSoulCntRend;
	m_eUISort = pDesc->eUISort;
	m_isNextLevel = pDesc->isNextlevel;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Position();

	return S_OK;
}

void CUI_StateSoul::Priority_Tick(_float fTimeDelta)
{
}

void CUI_StateSoul::Tick(_float fTimeDelta)
{
	if (!m_pPlayer)
	{
		list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
		m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
		Safe_AddRef(m_pPlayer);
	}
	else
	{
		// Player Level에 Ch_Upgrade 시 필요한 Soul 개수 계산하기
		m_wstrLevelSout = to_wstring((m_pPlayer->Get_Level() * 10) + 290);
	}


	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	if (m_isCalculRend)
	{
		m_fRenderTimer += fTimeDelta;
		if (2.f <= m_fRenderTimer)
		{
			m_fRenderTimer = 0.f;
			m_isCalculRend = false;
		}
	}
}

void CUI_StateSoul::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, m_eUISort);
}

HRESULT CUI_StateSoul::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	Rend_Font();

	return S_OK;
}

void CUI_StateSoul::Rend_Calcul(_int iSoul)
{
	m_isCalculRend = true;

	if(0 > iSoul)
		m_wstrCalculText = TEXT("-") + to_wstring(iSoul);
	else
		m_wstrCalculText = TEXT("+") + to_wstring(iSoul);
}

HRESULT CUI_StateSoul::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateSoul"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_StateSoul::Bind_ShaderResources()
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

void CUI_StateSoul::Rend_Font()
{
	if (m_isSoulCntRend)
		m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), to_wstring(CInventory::GetInstance()->Get_Soul()), _float2(m_fX + 20.f, m_fY - 10.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	else if (m_isNextLevel)
		m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), m_wstrLevelSout, _float2(m_fX + 20.f, m_fY - 10.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	if (m_isCalculRend)
		m_pGameInstance->Render_Font(TEXT("Font_Cardo15"), m_wstrCalculText, _float2(m_fX + 20.f, m_fY + 10.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
}

CUI_StateSoul* CUI_StateSoul::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_StateSoul* pInstance = new CUI_StateSoul(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_StateSoul");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_StateSoul::Clone(void* pArg)
{
	CUI_StateSoul* pInstance = new CUI_StateSoul(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_StateSoul");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_StateSoul::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
