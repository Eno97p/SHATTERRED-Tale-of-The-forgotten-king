#pragma once

#include "Base.h"
#include "Client_Defines.h"

#include "Item.h"
#include "ItemData.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
END

BEGIN(Client)

class CInventory final : public CBase
{
	DECLARE_SINGLETON(CInventory)

public:
	CInventory();
	virtual ~CInventory() = default;

public:
	CItemData*							Get_ItemData(_uint iSlotIdx);
	_uint								Get_vecItemSize() { return m_vecItem.size(); }
	_bool								Get_isQuickEmpty() { return m_mapQuickAccess.empty(); }
	_uint								Get_QuickMapSize() { return m_mapQuickAccess.size(); } // 추후 이름 바꿀 것
	map<_uint, class CItemData*>*		Get_QuickMap() { return &m_mapQuickAccess; }
	_uint								Get_WeaponSize() { return m_vecWeapon.size(); }
	_uint								Get_SkillSize() { return m_vecSkill.size(); }

	vector<class CItemData*>*			Get_ItemDatas(){ return&m_vecItem; }
	vector<class CItemData*>*			Get_Weapons() { return &m_vecWeapon; }
	class CItemData*					Get_EquipWeapon(_uint iArrIdx) { return m_arrEquipWeapon[iArrIdx]; }
	class CItemData*					Get_EquipSkill(_uint iArrIdx) { return m_arrEquipSkill[iArrIdx]; }

	_uint								Get_EquipWeaponSize();
	_uint								Get_EquipSkillSize();
	vector<class CItemData*>*			Get_Artefact() { return &m_vecArtefact; }
	vector<class CItemData*>*			Get_Skills() { return &m_vecSkill; }

	_uint								Get_Soul() { return m_iSoul; }

public:
	HRESULT Initialize();
	void	Tick(_float fTimeDelta);

	HRESULT	Initialize_DefaultItem();
	HRESULT	Add_DropItem(CItem::ITEM_NAME eItemType);
	HRESULT Add_Item(CItemData::ITEM_NAME eItemName, _bool isRend = true);
	HRESULT Add_Weapon(CItemData::ITEM_NAME eItemName, _bool isRend = true);
	HRESULT	Add_Skill(CItemData::ITEM_NAME eItemName, _bool isRend = true);
	HRESULT	Add_QuickAccess(CItemData* pItemData, _int iInvenIdx);
	HRESULT	Add_EquipWeapon(CItemData* pItemData, _uint iEquipSlotIdx);
	HRESULT	Add_EquipSkill(CItemData* pItemData, _uint iEquipSlotIdx);

	HRESULT	Delete_QuickAccess(_uint iInvenIdx, _uint iQuickIdx, wstring wstrItemName);
	HRESULT	Delete_EquipWeapon(_uint iEquipSlotIdx);
	HRESULT	Delete_EquipSkill(_uint iEquipSlotIdx);

	HRESULT	Delete_Item(CItemData* pItemData);

	void	Calcul_Soul(_int iSoul) { m_iSoul += iSoul; } // 인자로 넣어준 값을 기존 Soul에 연산해주기 (+ or -)
	_bool	Check_Overlab(CItem::ITEM_NAME eItemType);

private:
	CGameInstance* m_pGameInstance = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

private:
	_uint							m_iSoul = { 2000 }; // test 2000

	vector<class CItemData*>		m_vecItem;
	map<_uint, CItemData*>			m_mapQuickAccess;

	vector<class CItemData*>		m_vecWeapon;
	vector<class CItemData*>		m_vecSkill; // 보조 무기
	class CItemData*				m_arrEquipWeapon[3];
	class CItemData*				m_arrEquipSkill[3];
	vector<class CItemData*>		m_vecArtefact;

public:
	virtual void Free() override;
};

END