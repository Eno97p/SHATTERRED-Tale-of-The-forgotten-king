#include "UIGroup_BossHP.h"

#include "GameInstance.h"
#include "UI_BossHP.h"
#include "UI_BossHPBar.h"
#include "UI_BossShield.h"

CUIGroup_BossHP::CUIGroup_BossHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_BossHP::CUIGroup_BossHP(const CUIGroup_BossHP& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_BossHP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_BossHP::Initialize(void* pArg)
{
	UIGROUP_BOSSHP_DESC* pDesc = static_cast<UIGROUP_BOSSHP_DESC*>(pArg);

	m_eBossUIName = pDesc->eBossUIName;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	m_isRend = false;

	return S_OK;
}

void CUIGroup_BossHP::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_BossHP::Tick(_float fTimeDelta)
{
	_bool isRender_End = false;
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
		{
			if (!m_isRenderOnAnim && !(pUI->Get_RenderOnAnim()))
			{
				pUI->Resset_Animation(true);
			}
			else if (m_isRenderOnAnim && pUI->Get_RenderOnAnim())
			{
				pUI->Resset_Animation(false);
			}


			pUI->Tick(fTimeDelta);

			isRender_End = pUI->isRender_End();
		}
		if (isRender_End)
			m_isRend = false;

		if (nullptr != m_pShield)
			m_pShield->Tick(fTimeDelta);
	}

	vector<CUI*>::iterator hp = m_vecUI.begin();
	dynamic_cast<CUI_BossHP*>(*hp)->Set_Ratio(m_fHPRatio);

	if(nullptr != m_pShield)
		m_pShield->Set_Ratio(m_fShieldRatio);

	//m_isRend = true;
	// Shield에 대한 비율 갱신도 필요함
}

void CUIGroup_BossHP::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Late_Tick(fTimeDelta);

		if (nullptr != m_pShield)
			m_pShield->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_BossHP::Render()
{
	return S_OK;
}

void CUIGroup_BossHP::Rend_Damage(_int iValue)
{
	vector<CUI*>::iterator hp = m_vecUI.begin();
	dynamic_cast<CUI_BossHP*>(*hp)->Rend_Damage(iValue);
}

HRESULT CUIGroup_BossHP::Create_UI()
{
	CUI::UI_DESC pDesc{};
	// HP
	pDesc.eLevel = LEVEL_STATIC;
	m_vecUI.emplace_back(dynamic_cast<CUI_BossHP*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_BossHP"), &pDesc)));

	// Bar
	CUI_BossHPBar::UI_BOSSHPBAR_DESC pBarDesc{};

	pBarDesc.eLevel = LEVEL_STATIC;

	if (BOSSUI_JUGGULUS == m_eBossUIName)
	{
		pBarDesc.wszBossName = TEXT("RAS SAMRAH, GUILTY ONE");
	}
	else if (BOSSUI_MANTARI == m_eBossUIName)
	{
		pBarDesc.wszBossName = TEXT("MANTARI, THE WANDERING SPECTER");
	}
	else if (BOSSUI_YANTARI == m_eBossUIName)
	{
		pBarDesc.wszBossName = TEXT("KETHER, APOCRYPHAL WATCHER");
	}
	else if (BOSSUI_ANDRAS == m_eBossUIName)
	{
		pBarDesc.wszBossName = TEXT("ANDRAS, THE FORSAKEN KING");
	}
	else if (BOSSUI_MALKHEL == m_eBossUIName)
	{
		pBarDesc.wszBossName = TEXT("MALKHEL, JUDGE AND KING");
	}

	m_vecUI.emplace_back(dynamic_cast<CUI_BossHPBar*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_BossHPBar"), &pBarDesc)));

	return S_OK;
}

void CUIGroup_BossHP::Create_Shield()
{
	if (nullptr != m_pShield)
		return;

	CUI::UI_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;

	m_pShield = dynamic_cast<CUI_BossShield*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_BossShield"), &pDesc));
}

CUIGroup_BossHP* CUIGroup_BossHP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_BossHP* pInstance = new CUIGroup_BossHP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_BossHP");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_BossHP::Clone(void* pArg)
{
	CUIGroup_BossHP* pInstance = new CUIGroup_BossHP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_BossHP");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_BossHP::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);

	Safe_Release(m_pShield);
}
