#include "UIGroup_UpGPage.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Inventory.h"

#include "UI_UpGPageBG.h"
#include "UI_UpGPageTop.h"
#include "UI_UpGPage_NameBox.h"
#include "UI_UpGPage_Circle.h"
#include "UI_UpGPageBtn.h"
#include "UI_UpGPage_Slot.h"
#include "UI_UpGPage_MatSlot.h"
#include "UI_UpGPage_Value.h"
#include "UI_ItemIcon.h"

#include "UIGroup_UP_Completed.h"

CUIGroup_UpGPage::CUIGroup_UpGPage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_UpGPage::CUIGroup_UpGPage(const CUIGroup_UpGPage& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_UpGPage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_UpGPage::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_UpGPage::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_UpGPage::Tick(_float fTimeDelta)
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

		for (auto& pSlot : m_vecSlot)
		{
			if (!m_isRenderOnAnim && !(pSlot->Get_RenderOnAnim()))
			{
				pSlot->Resset_Animation(true);
			}
			else if (m_isRenderOnAnim && pSlot->Get_RenderOnAnim())
			{
				pSlot->Resset_Animation(false);
			}

			pSlot->Tick(fTimeDelta);
		}

		for (auto& pValue : m_ValuesSlot)
		{
			if (!m_isRenderOnAnim && !(pValue->Get_RenderOnAnim()))
			{
				pValue->Resset_Animation(true);
			}
			else if (m_isRenderOnAnim && pValue->Get_RenderOnAnim())
			{
				pValue->Resset_Animation(false);
			}

			dynamic_cast<CUI_UpGPage_Value*>(pValue)->Update_Value(m_iCurSlotIdx);
			pValue->Tick(fTimeDelta);
		}

		m_pItemIcon->Tick(fTimeDelta);

		if (nullptr != m_pCompletedPage)
		{
			if (m_pCompletedPage->Get_isEnd())
			{
				Safe_Release(m_pCompletedPage);
				m_pCompletedPage = nullptr;

				// 여기서 Inventory의 weapon을 강화하는 것으로? Level을 올려주기
				Upgrade_ItemLevel();


			}else
				m_pCompletedPage->Tick(fTimeDelta);
		}
	}

	Update_CurSlot();
	Update_ItemIcon();
}

void CUIGroup_UpGPage::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Late_Tick(fTimeDelta);

		for (auto& pSlot : m_vecSlot)
			pSlot->Late_Tick(fTimeDelta);

		for (auto& pValue : m_ValuesSlot)
			pValue->Late_Tick(fTimeDelta);

		m_pItemIcon->Late_Tick(fTimeDelta);

		if (nullptr != m_pCompletedPage)
			m_pCompletedPage->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_UpGPage::Render()
{
	return S_OK;
}

void CUIGroup_UpGPage::Add_WeaponList(_uint iWeaponIdx) // 몇 번째 weapon인지 받아와서 해당 slot에 넣어주기
{
	vector<CUI*>::iterator slot = m_vecSlot.begin();
	for (size_t i = 0; i < iWeaponIdx; ++i)
		++slot;

	dynamic_cast<CUI_UpGPage_Slot*>(*slot)->Create_ItemIcon();
	dynamic_cast<CUI_UpGPage_Slot*>(*slot)->Setting_SelectItemName();
}

HRESULT CUIGroup_UpGPage::Create_UI()
{
	CUI::UI_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;

	// BG
	m_vecUI.emplace_back(dynamic_cast<CUI_UpGPageBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPageBG"), &pDesc)));

	// Top
	m_vecUI.emplace_back(dynamic_cast<CUI_UpGPageTop*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPageTop"), &pDesc)));

	// NameBox 
	m_vecUI.emplace_back(dynamic_cast<CUI_UpGPage_NameBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPage_NameBox"), &pDesc)));

	// Circle 
	m_vecUI.emplace_back(dynamic_cast<CUI_UpGPage_Circle*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPage_Circle"), &pDesc)));

	// Btn 
	m_vecUI.emplace_back(dynamic_cast<CUI_UpGPageBtn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPageBtn"), &pDesc)));

	// MatSlot  
	CUI_UpGPage_MatSlot::UI_MATSLOT_DESC pMatSlotDesc{};

	pMatSlotDesc.eLevel = LEVEL_STATIC;
	pMatSlotDesc.eMatSlotType = CUI_UpGPage_MatSlot::MATSLOT_L;
	m_vecUI.emplace_back(dynamic_cast<CUI_UpGPage_MatSlot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPage_MatSlot"), &pMatSlotDesc)));

	pMatSlotDesc.eMatSlotType = CUI_UpGPage_MatSlot::MATSLOT_R;
	m_vecUI.emplace_back(dynamic_cast<CUI_UpGPage_MatSlot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPage_MatSlot"), &pMatSlotDesc)));

	// Value
	CUI_UpGPage_Value::UI_VALUE_DESC pValueDesc{};

	pValueDesc.eLevel = LEVEL_STATIC;
	pValueDesc.eValueType = CUI_UpGPage_Value::VALUE_SOUL;
	m_ValuesSlot.emplace_back(dynamic_cast<CUI_UpGPage_Value*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPage_Value"), &pValueDesc)));

	pValueDesc.eValueType = CUI_UpGPage_Value::VALUE_MATERIAL;
	m_ValuesSlot.emplace_back(dynamic_cast<CUI_UpGPage_Value*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPage_Value"), &pValueDesc)));

	if (FAILED(Create_Slot()))
		return E_FAIL;

	// ItemIcon (Center)
	CUI_ItemIcon::UI_ITEMICON_DESC pItemIconDesc{};
	pItemIconDesc.eLevel = LEVEL_STATIC;
	pItemIconDesc.fX = (g_iWinSizeX >> 1);
	pItemIconDesc.fY = (g_iWinSizeY >> 1) - 100.f;
	pItemIconDesc.fSizeX = 300.f;
	pItemIconDesc.fSizeY = 300.f;
	pItemIconDesc.eUISort = TENTH;
	pItemIconDesc.wszTexture = TEXT("Prototype_Component_Texture_ItemIcon_None"); // None

	m_pItemIcon = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pItemIconDesc));

	return S_OK;
}

HRESULT CUIGroup_UpGPage::Create_Slot()
{
	CUI_UpGPage_Slot::UI_UPGSLOT_DESC pDesc{};
	
	for (size_t i = 0; i < 4; ++i)
	{
		pDesc.eLevel = LEVEL_STATIC;
		pDesc.fX = 250.f;
		pDesc.fY = 230.f + (i * 80.f);
		pDesc.fSizeX = 341.3f; // 512
		pDesc.fSizeY = 85.3f; // 128
		pDesc.iSlotIdx = i;
		m_vecSlot.emplace_back(dynamic_cast<CUI_UpGPage_Slot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPage_Slot"), &pDesc)));
	}

	return S_OK;
}

void CUIGroup_UpGPage::Create_CompletedPage()
{
	// 빈 슬롯을 클릭한 경우 예외 처리
	vector<CUI*>::iterator slot = m_vecSlot.begin();
	for (size_t i = 0; i < m_iCurSlotIdx; ++i)
		++slot;

	if (dynamic_cast<CUI_UpGPage_Slot*>(*slot)->Check_ItemIconNull())
		return;

	// 추후 수급 재료가 충족되어야 업그레이드 되도록 로직 추가 필요
	if (!Check_Enough())
		return;
	else // Soul과 해당 아이템 개수 만큼 제거해야 함!
	{
		Calculate_UpgradeCost();
	}

	CUIGroup_UP_Completed::UIGROUP_COMPLETED_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.iCurSlotIdx = m_iCurSlotIdx;
	m_pCompletedPage = dynamic_cast<CUIGroup_UP_Completed*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UP_Completed"), &pDesc));

	return;
}

void CUIGroup_UpGPage::Update_CurSlot()
{
	// m_iCurSlotIdx에 맞춰서 상태들 갱신
	vector<CUI*>::iterator slot = m_vecSlot.begin();

	for (size_t i = 0; i < m_vecSlot.size(); ++i)
	{
		if (i == m_iCurSlotIdx)
		{
			dynamic_cast<CUI_UpGPage_Slot*>(*slot)->Set_Select(true);
		}
		else
		{
			dynamic_cast<CUI_UpGPage_Slot*>(*slot)->Set_Select(false);
		}

		++slot;
	}
}

void CUIGroup_UpGPage::Update_ItemIcon()
{
	vector<CUI*>::iterator slot = m_vecSlot.begin();
	for (size_t i = 0; i < m_iCurSlotIdx; ++i)
		++slot;

	_bool isItemNull = dynamic_cast<CUI_UpGPage_Slot*>(*slot)->Check_ItemIconNull();
	wstring wstrItemTexture;

	if (isItemNull) // waepon이 없다
	{
		wstrItemTexture = TEXT("Prototype_Component_Texture_ItemIcon_None");

	}
	else // 아이템이 있다면
	{
		vector<CItemData*>::iterator weapon = CInventory::GetInstance()->Get_Weapons()->begin();
		for (size_t i = 0; i < m_iCurSlotIdx; ++i)
			++weapon;

		wstrItemTexture = (*weapon)->Get_TextureName();
	}

	m_pItemIcon->Change_Texture(wstrItemTexture);
}

void CUIGroup_UpGPage::Upgrade_ItemLevel()
{
	vector<CItemData*>::iterator weapon = CInventory::GetInstance()->Get_Weapons()->begin();
	for (size_t i = 0; i < m_iCurSlotIdx; ++i)
		++weapon;

	(*weapon)->Set_Level(1);
}

_bool CUIGroup_UpGPage::Check_Enough()
{
	for (auto& value : m_ValuesSlot)
	{
		if (!dynamic_cast<CUI_UpGPage_Value*>(value)->Get_isEnough())
			return false;
	}

	return true;
}

void CUIGroup_UpGPage::Calculate_UpgradeCost()
{
	vector<CItemData*>::iterator weapon = CInventory::GetInstance()->Get_Weapons()->begin();
	for (size_t i = 0; i < m_iCurSlotIdx; ++i)
		++weapon;
	 
	// Soul
	CInventory::GetInstance()->Calcul_Soul(-(*weapon)->Get_Price());

	// Upgrade Material
	vector<CItemData*>::iterator item =  CInventory::GetInstance()->Get_ItemDatas()->begin();
	for (size_t i = 0; i < CInventory::GetInstance()->Get_vecItemSize(); ++i)
	{
		if ((*item)->Get_ItemNameText() == TEXT("HADRONITE"))
		{
			for(size_t j = 0; j < (*weapon)->Get_Value(); ++j)
				(*item)->Apply_UseCount(i);

			break;
		}
		else
			++item;
	}

}

CUIGroup_UpGPage* CUIGroup_UpGPage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_UpGPage* pInstance = new CUIGroup_UpGPage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_UpGPage");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_UpGPage::Clone(void* pArg)
{
	CUIGroup_UpGPage* pInstance = new CUIGroup_UpGPage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_UpGPage");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_UpGPage::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);

	for (auto& pSlot : m_vecSlot)
		Safe_Release(pSlot);

	for (auto& pValues : m_ValuesSlot)
		Safe_Release(pValues);

	Safe_Release(m_pItemIcon);
	Safe_Release(m_pCompletedPage);
}
