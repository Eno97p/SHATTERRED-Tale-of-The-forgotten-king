#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

#include "Item.h"

BEGIN(Engine)

END

// Item�� ���� ������ ������ ������ Ŭ����(��ü�� X) > Item ���� �� �����Ǹ� Inv�� ������ �ֵ��� > �ش� Ŭ������ ������ �����Ϳ� ���� Texture ��� �� ��ȣ�ۿ� �߻��ϵ���

BEGIN(Client)

class CItemData final : public CGameObject
{
public:
	enum ITEM_TYPE { ITEMTYPE_WEAPON, ITEMTYPE_BUFF, ITEMTYPE_SKILL, ITEMTYPE_USABLE, ITEMTYPE_UPGRADE, ITEMTYPE_ETC, ITEMTYPE_END };
	enum ITEM_NAME {
		ITEMNAME_DURGASWORD, ITEMNAME_PRETORIANSWORD, ITEMNAME_RADAMANTHESWORD, ITEMNAME_WHISPERSWORD,
		ITEMNAME_OPH, ITEMNAME_AKSHA,
		ITEMNAME_CATALYST,
		ITEMNAME_HOVERBOARD, ITEMNAME_FIREFLY,
		ITEMNAME_BUFF1, ITEMNAME_BUFF2, ITEMNAME_BUFF3, ITEMNAME_BUFF4, ITEMNAME_SOUL, ITEMNAME_ESSENCE, ITEMNAME_ETHER, ITEMNAME_UPGRADE1, ITEMNAME_UPGRADE2,
		ITEMNAME_END
	};

	typedef struct ItemData_Desc : public GAMEOBJECT_DESC
	{
		_bool					isDropTem; // ��ž�� ����
		CItem::ITEM_NAME		eDropItemName;
		ITEM_NAME				eItemName;

	}ITEMDATA_DESC;

private:
	CItemData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemData(const CItemData& rhs);
	virtual ~CItemData() = default;

public:
	_uint			Get_Count() { return m_iCount; }
	void			Set_Count(_int iCount) { m_iCount += iCount; }

	_bool			Get_isRedDotUse() { return m_isRedDotUse; }
	void			Set_isRedDotUse(_bool isRedDotUse) { m_isRedDotUse = isRedDotUse; }

public:
	wstring			Get_TextureName() { return m_wszTexture; }
	wstring			Get_ItemNameText() { return m_wszItemName; }
	wstring			Get_ItemExplainText() { return m_wszItemExplain; }
	wstring			Get_ItemExplainText_Quick() { return m_wszItemExplain_quick; }

	_bool			Get_isEquip() { return m_isEquip; }
	void			Set_isEquip(_bool isEquip) { m_isEquip = isEquip; }

	ITEM_NAME		Get_ItemName() { return m_eItemName; }
	ITEM_TYPE		Get_ItemType() { return m_eItemType; }

	_int			Get_Price() { return m_iPrice; }
	_uint			Get_Value() { return m_iValue; }
	_uint			Get_Level() { return m_iLevel; }
	void			Set_Level(_uint iAddLevel) { m_iLevel += iAddLevel; }
	_uint			Get_AddDamage() { return m_iAddDamage; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void			Use_Item(_uint iInvenIdx);
	void			Apply_UseCount(_uint iInvenIdx);
	void			Use_Skill();

private:
	_bool					m_isEquip = { false };
	_bool					m_isRedDotUse = { false }; // RedDot ���� ����(Skill�� �ش�)

	_uint					m_iCount = { 0 }; // ������ ����

#pragma region Weapon Upgrade Data // ��ȭ�� �ʿ��� �����͵� (weapon�� �ش�)
	_int					m_iPrice = { 0 }; // Soul ����
	_uint					m_iValue = { 0 }; // �ʿ� ��ȭ ��� ����
	_uint					m_iLevel = { 0 }; // Weapon ����
	_uint					m_iAddDamage = { 0 }; // ���� �� �����ϴ� ������
#pragma endregion Weapon Upgrade Data

	wstring					m_wszTexture = TEXT("");
	wstring					m_wszItemName = TEXT("");
	wstring					m_wszItemExplain = TEXT("");
	wstring					m_wszItemExplain_quick = TEXT("");

	ITEM_TYPE				m_eItemType = { ITEMTYPE_END };
	ITEM_NAME				m_eItemName = { ITEMNAME_END };

private:
	void					Set_DropItem_Data(CItem::ITEM_NAME eItemName);
	void					Set_Item_Data();

public:
	static CItemData*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END