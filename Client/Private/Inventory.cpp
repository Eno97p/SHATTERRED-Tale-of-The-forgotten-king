#include "Inventory.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Player.h"
#include "ItemData.h"

#include "UIGroup_DropItem.h"
#include "UIGroup_WeaponSlot.h"
#include "UIGroup_Weapon.h"
#include "UIGroup_UpGPage.h"
#include "UIGroup_State.h"

IMPLEMENT_SINGLETON(CInventory)

CInventory::CInventory()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CItemData* CInventory::Get_ItemData(_uint iSlotIdx)
{
	vector<CItemData*>::iterator item = m_vecItem.begin();

	if (m_vecItem.size() <= iSlotIdx)
		return nullptr;

	for (size_t i = 0; i < iSlotIdx; ++i)
	{
		++item;

	}

	return (*item);
}

_uint CInventory::Get_EquipWeaponSize()
{
	// ���� ���Կ� ���� �� ������ �� ������ ��ȯ�ϴ� �Լ�
	_uint iCount = { 0 };

	for (size_t i = 0; i < 3; ++i)
	{
		if (m_arrEquipWeapon[i] != nullptr)
			++iCount;
	}

	return iCount;
}

_uint CInventory::Get_EquipSkillSize()
{
	_uint iCount = { 0 };

	for (size_t i = 0; i < 3; ++i)
	{
		if (m_arrEquipSkill[i] != nullptr)
			++iCount;
	}

	return iCount;
}

HRESULT CInventory::Initialize()
{
	if (FAILED(Initialize_DefaultItem()))
		return E_FAIL;

    return S_OK;
}

void CInventory::Tick(_float fTimeDelta)
{
}

HRESULT CInventory::Initialize_DefaultItem()
{
	// ���� ó�� ���� �� �⺻������ ������ �ִ� ������ 
	// Weapon�� �߰�
	
	Add_Weapon(CItemData::ITEMNAME_RADAMANTHESWORD, false); 

	//Add_Weapon(CItemData::ITEMNAME_DURGASWORD, false);
	//Add_Weapon(CItemData::ITEMNAME_PRETORIANSWORD, false);
	//Add_Weapon(CItemData::ITEMNAME_WHISPERSWORD, false);

	CItemData::ITEMDATA_DESC pDesc{};

	pDesc.isDropTem = false;

	// Equip Sign Ȱ��ȭ
	vector<CItemData*>::iterator weapon = m_vecWeapon.begin();
	Add_EquipWeapon((*weapon), 0);
	(*weapon)->Set_isEquip(true);
	dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Update_Slot_EquipSign(true);

	// Skill
	Add_Skill(CItemData::ITEMNAME_OPH, false);
	//Add_Skill(CItemData::ITEMNAME_AKSHA);

	//test
	//Add_DropItem(CItem::ITEM_ESSENCE);
	//Add_DropItem(CItem::ITEM_BUFF1);
	//Add_DropItem(CItem::ITEM_BUFF1);
	//Add_DropItem(CItem::ITEM_BUFF1);
	//Add_DropItem(CItem::ITEM_BUFF2);
	//Add_DropItem(CItem::ITEM_BUFF3);
	//Add_DropItem(CItem::ITEM_BUFF4);
	//Add_DropItem(CItem::ITEM_UPGRADE1);
	//Add_DropItem(CItem::ITEM_UPGRADE1);
	//Add_DropItem(CItem::ITEM_UPGRADE1);
	//Add_DropItem(CItem::ITEM_UPGRADE1);
	////Add_DropItem(CItem::ITEM_ESSENCE);
	////Add_DropItem(CItem::ITEM_ETHER); 
	//Add_DropItem(CItem::ITEM_UPGRADE2);
	//
	for(size_t i = 0; i < 5; ++i)
		Add_Item(CItemData::ITEMNAME_CATALYST, false);

	//Add_Item(CItemData::ITEMNAME_HOVERBOARD);
	////Add_Item(CItemData::ITEMNAME_HOVERBOARD);
	//Add_Item(CItemData::ITEMNAME_FIREFLY);

	return S_OK;
}

HRESULT CInventory::Add_DropItem(CItem::ITEM_NAME eItemType)
{
	if (CItem::ITEM_SOUL == eItemType)
	{
		_uint iRand = rand() % 300 + 100;
		CInventory::GetInstance()->Calcul_Soul(iRand);

		// UI ��� ���� �Լ��� ȣ��
		dynamic_cast<CUIGroup_State*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_State"))->Rend_Calcul(iRand);
	}
	else
	{
		if (!Check_Overlab(eItemType)) // �ߺ� üũ
		{
			// Inventory�� ItemData �߰�
			CItemData::ITEMDATA_DESC pDesc{};

			pDesc.isDropTem = true;
			pDesc.eDropItemName = eItemType;
			m_vecItem.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

			CUI_Manager::GetInstance()->Update_Inventory_Add(m_vecItem.size() - 1);

			// Quick�� InvSlot���� ItemIcon ������־�� ��
			CUI_Manager::GetInstance()->Update_Quick_InvSlot_Add(m_vecItem.size() - 1);

			// UI DropItem ���
			CUIGroup_DropItem::UIGROUP_DROPITEM_DESC pUIDesc{};
			pUIDesc.eLevel = LEVEL_STATIC;

			vector<CItemData*>::iterator item = m_vecItem.begin();
			for (size_t i = 0; i < m_vecItem.size() - 1; ++i)
				++item;
			pUIDesc.eItemName = (*item)->Get_ItemName();
			pUIDesc.wszTextureName = (*item)->Get_TextureName();
			pUIDesc.isItem = true;
			m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UIGroup_DropItem"), &pUIDesc);
		
			// RedDot �߰�
			CUI_Manager::GetInstance()->Create_RedDot_MenuBtn(true);
			CUI_Manager::GetInstance()->Create_RedDot_Slot(true, m_vecItem.size() - 1); // Slot
		}
	}

	return S_OK;
}

HRESULT CInventory::Add_Item(CItemData::ITEM_NAME eItemName, _bool isRend)
{
	if (eItemName == CItemData::ITEMNAME_CATALYST)
	{
		CUIGroup_DropItem::UIGROUP_DROPITEM_DESC pDropItemDesc{};
		pDropItemDesc.eLevel = LEVEL_STATIC;

		vector<CItemData*>::iterator item = m_vecItem.begin();

		for (size_t i = 0; i < m_vecItem.size(); ++i)
		{
			if (CItemData::ITEMNAME_CATALYST == (*item)->Get_ItemName())
			{
				(*item)->Set_Count(1);

				pDropItemDesc.eItemName = (*item)->Get_ItemName();
				pDropItemDesc.wszTextureName = (*item)->Get_TextureName();
				pDropItemDesc.isItem = true;
				m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UIGroup_DropItem"), &pDropItemDesc);

				return S_OK;
			}
			else
				++item;
		}
	}

	// Inventory�� ItemData�߰�
	CItemData::ITEMDATA_DESC pDesc{};

	pDesc.isDropTem = false;
	pDesc.eItemName = eItemName;
	m_vecItem.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	if (isRend)
	{
		// UI ���
		CUIGroup_DropItem::UIGROUP_DROPITEM_DESC pUIDesc{};
		pUIDesc.eLevel = LEVEL_STATIC;

		vector<CItemData*>::iterator item = m_vecItem.begin();
		for (size_t i = 0; i < m_vecItem.size() - 1; ++i)
			++item;
		pUIDesc.eItemName = (*item)->Get_ItemName();
		pUIDesc.wszTextureName = (*item)->Get_TextureName();
		pUIDesc.isItem = true;
		m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UIGroup_DropItem"), &pUIDesc);
	}

	// UI��
	CUI_Manager::GetInstance()->Update_Inventory_Add(m_vecItem.size() - 1);

	// Quick�� InvSlot���� ItemIcon ������־�� ��
	CUI_Manager::GetInstance()->Update_Quick_InvSlot_Add(m_vecItem.size() - 1);

	// RedDot �߰�
	CUI_Manager::GetInstance()->Create_RedDot_MenuBtn(true); // Menu Btn
	CUI_Manager::GetInstance()->Create_RedDot_Slot(true, m_vecItem.size() - 1); // Slot

	return S_OK;
}

HRESULT CInventory::Add_Weapon(CItemData::ITEM_NAME eItemName, _bool isRend)
{
	CItemData::ITEMDATA_DESC pDesc{};
	pDesc.isDropTem = false;
	pDesc.eItemName = eItemName;
	m_vecWeapon.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	CUI_Manager::GetInstance()->Update_Weapon_Add(); 

	// Upgrade Page���� weapon �߰�
	dynamic_cast<CUIGroup_UpGPage*>(CUI_Manager::GetInstance()->Get_UIGroup("UpGPage"))->Add_WeaponList(m_vecWeapon.size() - 1);

	// RedDot
	CUI_Manager::GetInstance()->Create_RedDot_MenuBtn(false);
	CUI_Manager::GetInstance()->Create_RedDot_Slot(false, m_vecWeapon.size() - 1);

	// DropItem UI ���
	CUIGroup_DropItem::UIGROUP_DROPITEM_DESC pUIDesc{};
	pUIDesc.eLevel = LEVEL_STATIC;

	vector<CItemData*>::iterator weapon = m_vecWeapon.begin();
	for (size_t i = 0; i < m_vecWeapon.size() - 1; ++i)
		++weapon;
	pUIDesc.eItemName = (*weapon)->Get_ItemName();
	pUIDesc.wszTextureName = (*weapon)->Get_TextureName();
	pUIDesc.isItem = false;
	pUIDesc.isWeapon = true;
	pUIDesc.isRend = isRend;

	m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UIGroup_DropItem"), &pUIDesc);

	return S_OK;
}

HRESULT CInventory::Add_Skill(CItemData::ITEM_NAME eItemName, _bool isRend)
{
	CItemData::ITEMDATA_DESC pDesc{};

	pDesc.isDropTem = false;
	pDesc.eItemName = eItemName;

	m_vecSkill.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	// RedDot
	CUI_Manager::GetInstance()->Create_RedDot_MenuBtn(false);
	CUI_Manager::GetInstance()->Create_RedDot_Slot(false, m_vecSkill.size() - 1, true);

	// DropItem UI ���
	CUIGroup_DropItem::UIGROUP_DROPITEM_DESC pUIDesc{};
	pUIDesc.eLevel = LEVEL_STATIC;

	vector<CItemData*>::iterator skill = m_vecSkill.begin();
	for (size_t i = 0; i < m_vecSkill.size() - 1; ++i)
		++skill;
	pUIDesc.eItemName = (*skill)->Get_ItemName();
	pUIDesc.wszTextureName = (*skill)->Get_TextureName();
	pUIDesc.isItem = false;
	pUIDesc.isWeapon = false;
	pUIDesc.isRend = isRend;

	m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UIGroup_DropItem"), &pUIDesc);

	return S_OK;
}

HRESULT CInventory::Add_QuickAccess(CItemData* pItemData, _int iInvenIdx)
{
	// Inventory���� ���� ������ �������� QuickAccess�� ���
	m_mapQuickAccess.emplace(iInvenIdx, pItemData);

	// InvSub QuickAccess Slot UI�� ����ϱ�
	CUI_Manager::GetInstance()->Update_InvSub_Quick_Add(iInvenIdx);

	// QuickAccess���� ��� �ʿ�
	CUI_Manager::GetInstance()->Update_Quick_Add(pItemData, iInvenIdx);

	dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Update_QuickSlot(pItemData->Get_TextureName());

	return S_OK;
}

HRESULT CInventory::Add_EquipWeapon(CItemData* pItemData, _uint iEquipSlotIdx)
{
	// Weapon�� Slot���� ���� ������ �������� EquipWeapon�� �־��ֱ�
	m_arrEquipWeapon[iEquipSlotIdx] = pItemData;

	// Weapon Equip Slot UI�� ���
	CUI_Manager::GetInstance()->Update_EquipWeapon_Add(iEquipSlotIdx); // ���⼭ ��������

	// HUD
	dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Update_WeaponSlot(pItemData->Get_TextureName(), CUIGroup_WeaponSlot::SLOT_WEAPON);

	// Player�� �� Į�� �����ϱ�
	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	if (PlayerList.empty())
		return S_OK;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	// HUD�� Icon�� ���� Player�� ������ ��� �ִ� Weapon ����
	pPlayer->Update_Weapon(pItemData->Get_TextureName());

	return S_OK;
}

HRESULT CInventory::Delete_QuickAccess(_uint iInvenIdx, _uint iQuickIdx, wstring wstrItemName)
{
	// ItemData�� Equp ���� ����
	vector<CItemData*>::iterator item = m_vecItem.begin();
	for (size_t i = 0; i < m_vecItem.size(); ++i)
	{
		if (wstrItemName == (*item)->Get_ItemNameText())
		{
			(*item)->Set_isEquip(false);
			break;
		}
		else
			++item;
	}

	m_mapQuickAccess.erase(iInvenIdx);

	return S_OK;
}

HRESULT CInventory::Delete_EquipWeapon(_uint iEquipSlotIdx)
{
	// ���ڷ� ���� Slot�� ������ ����
	m_arrEquipWeapon[iEquipSlotIdx] = nullptr;

	// Weapon Equip Slot UI���� ����
	CUI_Manager::GetInstance()->Update_EquipWeapon_Delete(iEquipSlotIdx);

	// HUD������ ��� ���� 
	dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Reset_SlotTexture(CUIGroup_WeaponSlot::SLOT_WEAPON);

	return S_OK;
}

HRESULT CInventory::Add_EquipSkill(CItemData* pItemData, _uint iEquipSlotIdx)
{
	m_arrEquipSkill[iEquipSlotIdx] = pItemData;

	CUI_Manager::GetInstance()->Update_EquipWeapon_Add(iEquipSlotIdx);

	// HUD
	dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Update_WeaponSlot(pItemData->Get_TextureName(), CUIGroup_WeaponSlot::SLOT_SKILL);

	return S_OK;
}

HRESULT CInventory::Delete_EquipSkill(_uint iEquipSlotIdx)
{
	m_arrEquipSkill[iEquipSlotIdx] = nullptr;

	CUI_Manager::GetInstance()->Update_EquipSkill_Delete(iEquipSlotIdx);

	// HUD ���� �ڵ嵵 �߰��ؾ���
	dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Reset_SlotTexture(CUIGroup_WeaponSlot::SLOT_SKILL);

	return S_OK;
}

HRESULT CInventory::Delete_Item(CItemData* pItemData)
{
	vector<CItemData*>::iterator item = m_vecItem.begin();
	for (size_t i = 0; i < m_vecItem.size(); ++i)
	{
		if (pItemData == (*item))
		{
			// Quick�� ���� ���̾��ٸ� �� ���� �������־�� ��
			if ((*item)->Get_isEquip())
			{
				map<_uint, CItemData*>::iterator quickItem = m_mapQuickAccess.begin();
				for (size_t j = 0; j < m_mapQuickAccess.size(); ++j)
				{
					if ((*quickItem).second == (*item))
					{
						m_mapQuickAccess.erase(quickItem);
						break;
					}
					else
						++quickItem;
				}
			}

			Safe_Release((*item));
			m_vecItem.erase(item);
			break;
		}
		else
		{
			++item;
		}
	}

	return S_OK;
}

_bool CInventory::Check_Overlab(CItem::ITEM_NAME eItemType)
{
	CItemData::ITEM_NAME eItemName = { CItemData::ITEMNAME_END };
	switch (eItemType)
	{
	case Client::CItem::ITEM_BUFF1:
		eItemName = CItemData::ITEMNAME_BUFF1;
		break;
	case Client::CItem::ITEM_BUFF2:
		eItemName = CItemData::ITEMNAME_BUFF2;
		break;
	case Client::CItem::ITEM_BUFF3:
		eItemName = CItemData::ITEMNAME_BUFF3;
		break;
	case Client::CItem::ITEM_BUFF4:
		eItemName = CItemData::ITEMNAME_BUFF4;
		break;
	case Client::CItem::ITEM_ESSENCE:
		eItemName = CItemData::ITEMNAME_ESSENCE;
		break;
	case Client::CItem::ITEM_ETHER:
		eItemName = CItemData::ITEMNAME_ETHER;
		break;
	case Client::CItem::ITEM_UPGRADE1:
		eItemName = CItemData::ITEMNAME_UPGRADE1;
		break;
	case Client::CItem::ITEM_UPGRADE2:
		eItemName = CItemData::ITEMNAME_UPGRADE2;
		break;
	default:
		break;
	}

	// UI ���
	CUIGroup_DropItem::UIGROUP_DROPITEM_DESC pUIDesc{};
	pUIDesc.eLevel = LEVEL_STATIC;

	vector<CItemData*>::iterator item = m_vecItem.begin();

	for (size_t i = 0; i < m_vecItem.size(); ++i)
	{
		if (eItemName == (*item)->Get_ItemName()) // �̸��� ��ġ�ϴ� ���� �ִٸ�
		{
			// ���⼭ �ش� ItemData�� �ߺ� ó��
			(*item)->Set_Count(1);

			// UI DropItem ���
			pUIDesc.eItemName = (*item)->Get_ItemName();
			pUIDesc.wszTextureName = (*item)->Get_TextureName();
			pUIDesc.isItem = true;
			m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UIGroup_DropItem"), &pUIDesc);

			// Slot�� RedDot ����
			CUI_Manager::GetInstance()->Create_RedDot_MenuBtn(true);
			CUI_Manager::GetInstance()->Create_RedDot_Slot(true, i);

			return true;
		}
		else
		{
			++item;
		}
	}

	return false;
}

void CInventory::Free()
{
	for (auto& pItemData : m_vecItem)
		Safe_Release(pItemData);

	for (auto& pWeaponItem : m_vecWeapon)
		Safe_Release(pWeaponItem);

	for (auto& pSkillItem : m_vecSkill)
		Safe_Release(pSkillItem);

	for (auto& pArtefact : m_vecArtefact)
		Safe_Release(pArtefact);

	Safe_Release(m_pPlayer);
	Safe_Release(m_pGameInstance);
}
