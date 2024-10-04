#include "UIGroup_Weapon.h"

#include "GameInstance.h"
#include "Inventory.h"

#include "UI_MenuPageBG.h"
#include "UI_MenuPageTop.h"
#include "UI_QuickExplain.h"
#include "UI_Slot.h"
#include "UI_QuickInvBG.h"
#include "UI_WeaponRTop.h"
#include "UI_WPEquipSlot.h"
#include "UI_WPFontaine.h"
#include "UI_WeaponTab.h"
#include "UI_MenuAlphaBG.h"
#include "UI_MenuPage_BGAlpha.h"

CUIGroup_Weapon::CUIGroup_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_Weapon::CUIGroup_Weapon(const CUIGroup_Weapon& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_Weapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_Weapon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	m_eTabType = TAB_L; // 기본 : Weapon

	return S_OK;
}

void CUIGroup_Weapon::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_Weapon::Tick(_float fTimeDelta)
{
	_bool isRender_End = false;
	if (m_isRend)
	{
		if (m_pGameInstance->Key_Down(DIK_TAB))
		{
			Change_Tab();
		}

		if (!m_isEquipMode)
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
		}
		else
		{
			m_pAlphaBG->Tick(fTimeDelta);
		}

		for (auto& pEquipSlot : m_vecEquipSlot)
		{
			if (!m_isRenderOnAnim && !(pEquipSlot->Get_RenderOnAnim()))
			{
				pEquipSlot->Resset_Animation(true);
			}
			else if (m_isRenderOnAnim && pEquipSlot->Get_RenderOnAnim())
			{
				pEquipSlot->Resset_Animation(false);
			}
			pEquipSlot->Tick(fTimeDelta);
		}
	}
}

void CUIGroup_Weapon::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Late_Tick(fTimeDelta);

		for (auto& pSlot : m_vecSlot)
			pSlot->Late_Tick(fTimeDelta);

		for (auto& pEquipSlot : m_vecEquipSlot)
			pEquipSlot->Late_Tick(fTimeDelta);

		if(m_isEquipMode)
			m_pAlphaBG->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_Weapon::Render()
{
	return S_OK;
}

void CUIGroup_Weapon::Update_Weapon_Add()
{
	vector<CUI_Slot*>::iterator slot = m_vecSlot.begin();
	for (size_t i = 0; i < CInventory::GetInstance()->Get_WeaponSize() - 1; ++i)
		++slot;

	(*slot)->Create_ItemIcon_Weapon();
}

void CUIGroup_Weapon::Update_EquipSlot_Add(_uint iEquipSlotIdx)
{
	vector<CUI_WPEquipSlot*>::iterator equipslot = m_vecEquipSlot.begin();
	for (size_t i = 0; i < iEquipSlotIdx; ++i)
		++equipslot;

	if (TAB_L == m_eTabType)
	{
		(*equipslot)->Create_ItemIcon(true);
	}
	else if (TAB_R == m_eTabType)
	{
		(*equipslot)->Create_ItemIcon(false);
	}
}

void CUIGroup_Weapon::Update_EquipSlot_Delete(_uint iEquipSlotIdx)
{
	// Weapon Equip Slot에서 제거할 것
	vector<CUI_WPEquipSlot*>::iterator equipslot = m_vecEquipSlot.begin();
	for (size_t i = 0; i < iEquipSlotIdx; ++i)
		++equipslot;

	(*equipslot)->Delete_ItemIcon();
}

void CUIGroup_Weapon::Update_Skill_Add()
{
	vector<CUI_Slot*>::iterator slot = m_vecSlot.begin();
	for (size_t i = 0; i < CInventory::GetInstance()->Get_SkillSize() - 1; ++i)
		++slot;

	(*slot)->Create_ItemIcon_Skill();
}

void CUIGroup_Weapon::Update_Slot_EquipSign(_bool isEquip, _uint iSlotIdx)
{
	_uint iCount = { 0 };
	if (isEquip)
		iCount = m_iCurSlotIdx;
	else
		iCount = iSlotIdx;

	vector<CUI_Slot*>::iterator slot = m_vecSlot.begin();
	for (size_t i = 0; i < iCount; ++i)
		++slot;

	(*slot)->Set_isEquip(isEquip);
}

HRESULT CUIGroup_Weapon::Create_UI()
{
	CUI::UI_DESC pDesc{};

	// BG
	pDesc.eLevel = LEVEL_STATIC;
	m_vecUI.emplace_back(dynamic_cast<CUI_MenuPageBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MenuPageBG"), &pDesc)));

	// Top
	CUI_MenuPageTop::UI_TOP_DESC pTopDesc{};
	pTopDesc.eLevel = LEVEL_STATIC;
	pTopDesc.eTopType = CUI_MenuPageTop::TOP_WEAPON;
	m_vecUI.emplace_back(dynamic_cast<CUI_MenuPageTop*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MenuPageTop"), &pTopDesc)));

	// InvBG
	CUI_QuickInvBG::UI_INVBG_DESC pInvBGDesc{};
	pInvBGDesc.eLevel = LEVEL_STATIC;
	pInvBGDesc.isInv = true;
	m_vecUI.emplace_back(dynamic_cast<CUI_QuickInvBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_QuickInvBG"), &pInvBGDesc)));

	// RTop
	pDesc.eLevel = LEVEL_STATIC;
	m_vecUI.emplace_back(dynamic_cast<CUI_WeaponRTop*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WeaponRTop"), &pDesc)));

	Create_Tab();
	Create_Slot();

	// Alpha BG // 9번째로 깔아보기
	CUI_MenuPage_BGAlpha::UI_MP_BGALPHA_DESC pBGDesc{};
	pBGDesc.eLevel = LEVEL_STATIC;
	pBGDesc.eUISort = TENTH;
	m_pAlphaBG = dynamic_cast<CUI_MenuPage_BGAlpha*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MenuPage_BGAlpha"), &pBGDesc));

	// Explain
	CUI_QuickExplain::UI_EXPLAIN_DESC pExplainDesc{};
	pExplainDesc.eLevel = LEVEL_STATIC;
	pExplainDesc.eUISort = TENTH;
	m_vecUI.emplace_back(dynamic_cast<CUI_QuickExplain*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_QuickExplain"), &pExplainDesc)));

	Create_Fontaine();
	Create_EquipSlot();

	return S_OK;
}

HRESULT CUIGroup_Weapon::Create_Slot()
{
	CUI_Slot::UI_SLOT_DESC pDesc{};

	for (size_t i = 0; i < 3; ++i)
	{
		for (size_t j = 0; j < 5; ++j)
		{
			ZeroMemory(&pDesc, sizeof(pDesc));
			pDesc.iSlotIdx = (i * j) + j;
			pDesc.eLevel = LEVEL_STATIC;
			pDesc.fX = 230.f + (j * 76.f); // 160
			pDesc.fY = 200.f + (i * 76.f);
			pDesc.fSizeX = 85.3f;
			pDesc.fSizeY = 85.3f;
			pDesc.eUISort = NINETH;
			pDesc.eSlotType = CUI_Slot::SLOT_WEAPON;
			m_vecSlot.emplace_back(dynamic_cast<CUI_Slot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Slot"), &pDesc)));
		}
	}

	return S_OK;
}

HRESULT CUIGroup_Weapon::Create_EquipSlot()
{
	CUI_WPEquipSlot::UI_EQUIPSLOT_DESC pDesc{};

	CUI_WPEquipSlot::SLOT_NUM arrSlotNum[CUI_WPEquipSlot::NUM_END]
		= { CUI_WPEquipSlot::NUM_ONE,  CUI_WPEquipSlot::NUM_TWO, CUI_WPEquipSlot::NUM_THREE };

	for (size_t i = 0; i < 3; ++i)
	{
		ZeroMemory(&pDesc, sizeof(pDesc));
		pDesc.eLevel = LEVEL_STATIC;
		pDesc.eSlotNum = arrSlotNum[i];
		m_vecEquipSlot.emplace_back(dynamic_cast<CUI_WPEquipSlot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WPEquipSlot"), &pDesc)));
	}

	return S_OK;
}

HRESULT CUIGroup_Weapon::Create_Fontaine()
{
	CUI_WPFontaine::UI_FONTAINE_DESC pDesc{};

	for (size_t i = 0; i < 3; ++i)
	{
		ZeroMemory(&pDesc, sizeof(pDesc));
		pDesc.eLevel = LEVEL_STATIC;
		pDesc.iSlotNum = i;
		m_vecUI.emplace_back(dynamic_cast<CUI_WPFontaine*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WPFontaine"), &pDesc)));
	}

	return S_OK;
}

HRESULT CUIGroup_Weapon::Create_Tab()
{
	CUI_WeaponTab::UI_TAB_DESC	pDesc{};
	CUI_WeaponTab::TAB_TYPE		arrType[CUI_WeaponTab::TAB_END] = { CUI_WeaponTab::TAB_L, CUI_WeaponTab::TAB_R };

	for (size_t i = 0; i < 2; ++i)
	{
		ZeroMemory(&pDesc, sizeof(pDesc));
		pDesc.eLevel = LEVEL_STATIC;
		pDesc.eTabType = arrType[i];
		m_vecUI.emplace_back(dynamic_cast<CUI_WeaponTab*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WeaponTab"), &pDesc)));
	}

	return S_OK;
}

void CUIGroup_Weapon::Change_Tab()
{
	// Tab Change 할 때 Equip Sign도 다시 적용되어야 함!

	if (TAB_L == m_eTabType)
	{
		// Slot들 리셋
		vector<CUI_Slot*>::iterator slot = m_vecSlot.begin();
		for (size_t i = 0; i < CInventory::GetInstance()->Get_WeaponSize(); ++i)
		{
			(*slot)->Resset_Data();
			++slot;
		}

		// Skill의 값들로 Slot 채워주기 (Equip Slot에 값이 있다면 이것 또한 넣어주어야 함)
		slot = m_vecSlot.begin();
		for (size_t i = 0; i < CInventory::GetInstance()->Get_SkillSize(); ++i)
		{
			(*slot)->Change_ItemIcon_Skill();
			++slot;
		}

		// Equip Slot 리셋하고 채워넣기
		vector<CUI_WPEquipSlot*>::iterator equipslot = m_vecEquipSlot.begin();
		for (size_t i = 0; i < 3; ++i)
		{
			(*equipslot)->Delete_ItemIcon();
			(*equipslot)->Change_ItemIcon(false, i);
			++equipslot;
		}

		m_eTabType = TAB_R;
	}
	else if (TAB_R == m_eTabType)
	{
		// Slot들 리셋
		vector<CUI_Slot*>::iterator slot = m_vecSlot.begin();
		for (size_t i = 0; i < CInventory::GetInstance()->Get_SkillSize(); ++i)
		{
			(*slot)->Resset_Data();
			++slot;
		}

		slot = m_vecSlot.begin();
		for (size_t i = 0; i < CInventory::GetInstance()->Get_WeaponSize(); ++i)
		{
			(*slot)->Change_ItemIcon_Weapon(); // 여기서 몇 번째 슬롯인지 넣어줄                                                 까?
			++slot;
		}

		// Equip Slot 리셋하고 채워넣기
		vector<CUI_WPEquipSlot*>::iterator equipslot = m_vecEquipSlot.begin();
		for (size_t i = 0; i < 3; ++i)
		{
			(*equipslot)->Delete_ItemIcon();
			(*equipslot)->Change_ItemIcon(true, i);
			++equipslot;
		}

		m_eTabType = TAB_L;
	}
}

void CUIGroup_Weapon::Reset_Tab()
{
	// 만약 Tab이 Skill 상태라면 Weapon으로 되돌리기
	if (TAB_R == m_eTabType)
	{
		// Slot들 리셋
		vector<CUI_Slot*>::iterator slot = m_vecSlot.begin();
		for (size_t i = 0; i < CInventory::GetInstance()->Get_SkillSize(); ++i)
		{
			(*slot)->Resset_Data();
			++slot;
		}

		slot = m_vecSlot.begin();
		for (size_t i = 0; i < CInventory::GetInstance()->Get_WeaponSize(); ++i)
		{
			(*slot)->Change_ItemIcon_Weapon(); // 여기서 몇 번째 슬롯인지 넣어줄                                                 까?
			++slot;
		}

		// Equip Slot 리셋하고 채워넣기
		vector<CUI_WPEquipSlot*>::iterator equipslot = m_vecEquipSlot.begin();
		for (size_t i = 0; i < 3; ++i)
		{
			(*equipslot)->Delete_ItemIcon();
			(*equipslot)->Change_ItemIcon(true, i);
			++equipslot;
		}

		m_eTabType = TAB_L;
	}
}

HRESULT CUIGroup_Weapon::Create_RedDot(_uint iSlotIdx, _bool isSkill)
{
	if (!isSkill) // Weapon의 경우
	{
		vector<CUI_Slot*>::iterator slot = m_vecSlot.begin();
		for (size_t i = 0; i < iSlotIdx; ++i)
			++slot;

		return 	(*slot)->Create_RedDot();
	}
	else // Skill의 경우
	{
		vector<CItemData*>::iterator skill = CInventory::GetInstance()->Get_Skills()->begin();
		for (size_t i = 0; i < iSlotIdx; ++i)
			++skill;

		(*skill)->Set_isRedDotUse(true);
	}
}

HRESULT CUIGroup_Weapon::Delete_RedDot()
{
	for (auto& pSlot : m_vecSlot)
		pSlot->Delete_RedDot();

	return S_OK;
}

_bool CUIGroup_Weapon::Check_RedDot()
{
	// Skill에 RedDot이 하나라도 있는지 체크
	vector<CItemData*>::iterator skill = CInventory::GetInstance()->Get_Skills()->begin();
	for (size_t i = 0; i < CInventory::GetInstance()->Get_Skills()->size(); ++i)
	{
 		if ((*skill)->Get_isRedDotUse())
		{
			return true;
		}
		else
			++skill;
	}

	return false;
}

CUIGroup_Weapon* CUIGroup_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_Weapon* pInstance = new CUIGroup_Weapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_Weapon");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_Weapon::Clone(void* pArg)
{
	CUIGroup_Weapon* pInstance = new CUIGroup_Weapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_Weapon");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_Weapon::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);

	for (auto& pSlot : m_vecSlot)
		Safe_Release(pSlot);

	for (auto& pEquipSlot : m_vecEquipSlot)
		Safe_Release(pEquipSlot);

	Safe_Release(m_pAlphaBG);
}
