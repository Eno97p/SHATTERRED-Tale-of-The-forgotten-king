#include "UIGroup_BossText.h"

#include "GameInstance.h"

#include "UI_BossTextBG.h"
#include "UI_BossTextBox.h"

CUIGroup_BossText::CUIGroup_BossText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_BossText::CUIGroup_BossText(const CUIGroup_BossText& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_BossText::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_BossText::Initialize(void* pArg)
{
	UIGROUP_BOSSTEXT_DESC* pDesc = static_cast<UIGROUP_BOSSTEXT_DESC*>(pArg);

	m_isCreateText = pDesc->isCreateText;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_BossText::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_BossText::Tick(_float fTimeDelta)
{
	if (!m_vecUI.empty())
		Delete_UI();

	for (auto& pUI : m_vecUI)
	{
		if (nullptr != pUI)
			pUI->Tick(fTimeDelta);
	}
}

void CUIGroup_BossText::Late_Tick(_float fTimeDelta)
{
	for (auto& pUI : m_vecUI)
	{
		if (nullptr != pUI)
			pUI->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_BossText::Render()
{
	return S_OK;
}

HRESULT CUIGroup_BossText::Create_UI()
{
	CUI::UI_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;

	// BG
	m_vecUI.emplace_back(dynamic_cast<CUI_BossTextBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_BossTextBG"), &pDesc)));

	CUI_BossTextBox::UI_BOSSTEXTBOX_DESC pTextDesc{};
	pTextDesc.eLevel = LEVEL_STATIC;
	pTextDesc.isCreateText = m_isCreateText;
	// TextBox 
	m_vecUI.emplace_back(dynamic_cast<CUI_BossTextBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_BossTextBox"), &pTextDesc)));

	return S_OK;
}

void CUIGroup_BossText::Delete_UI()
{
	vector<CUI*>::iterator ui = m_vecUI.begin();
	if ((*ui) != nullptr && dynamic_cast<CUI_BossTextBG*>(*ui)->Get_isDead())
	{
		Safe_Release(*ui);
		(*ui) = nullptr;
	}

	++ui;
	if ((*ui) != nullptr && dynamic_cast<CUI_BossTextBox*>(*ui)->Get_isDead())
	{
		Safe_Release(*ui);
		(*ui) = nullptr;
	}
}

CUIGroup_BossText* CUIGroup_BossText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_BossText* pInstance = new CUIGroup_BossText(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_BossText");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_BossText::Clone(void* pArg)
{
	CUIGroup_BossText* pInstance = new CUIGroup_BossText(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_BossText");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_BossText::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);
}
