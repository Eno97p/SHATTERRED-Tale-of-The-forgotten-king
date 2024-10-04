#include "UIGroup_Menu.h"

#include "GameInstance.h"

#include "UI_MenuBG.h"
#include "UI_MenuAlphaBG.h"
#include "UI_MenuFontaine.h"
#include "UI_MenuBtn.h"
#include "UI_Menu_SelectFrame.h"

CUIGroup_Menu::CUIGroup_Menu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{pDevice, pContext}
{
}

CUIGroup_Menu::CUIGroup_Menu(const CUIGroup_Menu& rhs)
	: CUIGroup{rhs}
{
}

HRESULT CUIGroup_Menu::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_Menu::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_Menu::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_Menu::Tick(_float fTimeDelta)
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

		for (auto& pBtn : m_vecBtn)
		{
			if (!m_isRenderOnAnim && !(pBtn->Get_RenderOnAnim()))
			{
				pBtn->Resset_Animation(true);
			}
			else if (m_isRenderOnAnim && pBtn->Get_RenderOnAnim())
			{
				pBtn->Resset_Animation(false);
			}

			pBtn->Tick(fTimeDelta);
		}
	}
}

void CUIGroup_Menu::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Late_Tick(fTimeDelta);

		for (auto& pBtn : m_vecBtn)
			pBtn->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_Menu::Render()
{
	return S_OK;
}

HRESULT CUIGroup_Menu::Create_RedDot_MenuBtn(_bool isInv)
{
	for (auto& pBtn : m_vecBtn)
	{
		if ((isInv && pBtn->Get_MenuType() == CUI_MenuBtn::MENU_INV) || (!isInv && pBtn->Get_MenuType() == CUI_MenuBtn::MENU_WEAPON))
		{
			pBtn->Create_RedDot();
			break;
		}
	}

	return S_OK;
}

HRESULT CUIGroup_Menu::Delete_RedDot_MenuBtn(_bool isInv)
{
	for (auto& pBtn : m_vecBtn)
	{
		if (isInv)
		{
			if (pBtn->Get_MenuType() == CUI_MenuBtn::MENU_INV)
			{
				pBtn->Delete_RedDot();
				return S_OK;
			}
		}
		else
		{
			if (pBtn->Get_MenuType() == CUI_MenuBtn::MENU_WEAPON)
			{
				pBtn->Delete_RedDot();
				return S_OK;
			}
		}

	}
}

void CUIGroup_Menu::Set_RedDot_Rend(_bool isRend)
{
	for (auto& pBtn : m_vecBtn)
	{
		if (pBtn->Get_MenuType() == CUI_MenuBtn::MENU_INV || pBtn->Get_MenuType() == CUI_MenuBtn::MENU_WEAPON)
		{
			pBtn->Set_RedDot_Rend(isRend);
		}
	}
}

HRESULT CUIGroup_Menu::Create_UI()
{
	CUI::UI_DESC pDesc;

	// Menu BG
	pDesc.eLevel = LEVEL_STATIC;
	m_vecUI.emplace_back(dynamic_cast<CUI_MenuBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MenuBG"), &pDesc)));

	// Menu AlphaBG
	m_vecUI.emplace_back(dynamic_cast<CUI_MenuAlphaBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MenuAlphaBG"), &pDesc)));

	// Menu Fontaine
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.eLevel = LEVEL_STATIC;
	m_vecUI.emplace_back(dynamic_cast<CUI_MenuFontaine*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MenuFontaine"), &pDesc)));

	// MenuBtn
	CUI_MenuBtn::UI_MENUBTN_DESC pBtnDesc{};

	vector<CUI_MenuBtn::MENU_TYPE> vecMenuType =
		{ CUI_MenuBtn::MENU_CH, CUI_MenuBtn::MENU_MAP, CUI_MenuBtn::MENU_WEAPON, CUI_MenuBtn::MENU_INV, CUI_MenuBtn::MENU_SET, CUI_MenuBtn::MENU_COD };

	for (auto& pMenuType : vecMenuType)
	{
		//ZeroMemory(&pBtnDesc, sizeof(pBtnDesc));
		pBtnDesc.eLevel = LEVEL_STATIC;
		pBtnDesc.eMenuType = pMenuType;

		m_vecBtn.emplace_back(dynamic_cast<CUI_MenuBtn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MenuBtn"), &pBtnDesc))); // m_vecBtn
	}

	// Menu SelectFrame
	CUI_Menu_SelectFrame::UI_MENUSELECTFRAME_DESC pFrameDesc{};

	vector<CUI_Menu_SelectFrame::MENU_TYPE> vecFrameType =
				{ CUI_Menu_SelectFrame::MENU_CH, CUI_Menu_SelectFrame::MENU_MAP, CUI_Menu_SelectFrame::MENU_WEAPON,
					CUI_Menu_SelectFrame::MENU_INV, CUI_Menu_SelectFrame::MENU_SET, CUI_Menu_SelectFrame::MENU_COD };

	for (auto& pFrameType : vecFrameType)
	{
		ZeroMemory(&pFrameDesc, sizeof(pFrameDesc));
		pFrameDesc.eLevel = LEVEL_STATIC;
		pFrameDesc.eMenuType = pFrameType;

		m_vecUI.emplace_back(dynamic_cast<CUI_Menu_SelectFrame*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Menu_SelectFrame"), &pFrameDesc)));
	}

	return S_OK;
}

CUIGroup_Menu* CUIGroup_Menu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_Menu* pInstance = new CUIGroup_Menu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_Menu");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_Menu::Clone(void* pArg)
{
	CUIGroup_Menu* pInstance = new CUIGroup_Menu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_Menu");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_Menu::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);

	for (auto& pBtn : m_vecBtn)
		Safe_Release(pBtn);
}
