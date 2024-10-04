#include "UIGroup_Level.h"

#include "GameInstance.h"

#include "UI_LevelBG.h"
#include "UI_Level_TextBox.h"

CUIGroup_Level::CUIGroup_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_Level::CUIGroup_Level(const CUIGroup_Level& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_Level::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_Level::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_Level::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_Level::Tick(_float fTimeDelta)
{
	if (!m_vecUI.empty())
		Delete_UI();

	for (auto& pUI : m_vecUI)
	{
		if(nullptr != pUI)
			pUI->Tick(fTimeDelta);
	}
}

void CUIGroup_Level::Late_Tick(_float fTimeDelta)
{
	for (auto& pUI : m_vecUI)
	{
		if (nullptr != pUI)
			pUI->Late_Tick(fTimeDelta); 
	}
}

HRESULT CUIGroup_Level::Render()
{
	return S_OK;
}

HRESULT CUIGroup_Level::Create_UI()
{
	CUI::UI_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;

	// BG
	m_vecUI.emplace_back(dynamic_cast<CUI_LevelBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_LevelBG"), &pDesc)));

	// TextBox
	m_vecUI.emplace_back(dynamic_cast<CUI_Level_TextBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Level_TextBox"), &pDesc)));

	return S_OK;
}

void CUIGroup_Level::Delete_UI()
{
	vector<CUI*>::iterator ui = m_vecUI.begin();
	if ((*ui) != nullptr && dynamic_cast<CUI_LevelBG*>(*ui)->Get_isDead())
	{
		Safe_Release(*ui);
		(*ui) = nullptr;
	}

	++ui;
	if ((*ui) != nullptr && dynamic_cast<CUI_Level_TextBox*>(*ui)->Get_isDead())
	{
		Safe_Release(*ui);
		(*ui) = nullptr;
	}
}

CUIGroup_Level* CUIGroup_Level::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_Level* pInstance = new CUIGroup_Level(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_Level");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_Level::Clone(void* pArg)
{
	CUIGroup_Level* pInstance = new CUIGroup_Level(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_Level");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_Level::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);
}
