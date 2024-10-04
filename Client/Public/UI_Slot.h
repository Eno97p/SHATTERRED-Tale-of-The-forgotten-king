#pragma once

#include "UI_Interaction.h"
#include "UIGroup_Weapon.h"

BEGIN(Client)
class CUI_Slot_Frame;
class CUI_ItemIcon;
class CUI_Slot_EquipSign;
class CItemData;

class CUI_Slot final : public CUI_Interaction
{
public:
	enum SLOT_TYPE { SLOT_QUICK, SLOT_QUICKINV, SLOT_INV, SLOT_WEAPON, SLOT_INVSUB, SLOT_END };
	typedef struct UI_Slot_Desc : public UI_DESC
	{
		_uint						iSlotIdx;
		UISORT_PRIORITY				eUISort;
		SLOT_TYPE					eSlotType;
	}UI_SLOT_DESC;

private:
	CUI_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Slot(const CUI_Slot& rhs);
	virtual ~CUI_Slot() = default;

public:
	_bool			Get_isEquip() { return m_isEquip; }
	void			Set_isEquip(_bool isEquip) { m_isEquip = isEquip; }
	
	_bool			Get_isItemIconNull()
	{ 
		if (m_pItemIcon != nullptr)
			return false;
		else
			return true;
	}

	_uint			Get_SlotIdx() { return m_iSlotIdx; }
	_uint			Get_InvenIdx() { return m_iInventoryIdx; }

	wstring			Get_Texture() { return m_wstrTexture; }
	wstring			Get_ItemName() { return m_wstrMoDelName; }
	wstring			Get_ItemExplain() { return m_wszItemExplain; }
	wstring			Get_ItemExplain_Quick() { return m_wszItemExplain_Quick; }


public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	HRESULT			Create_ItemIcon_Inv();
	HRESULT			Create_ItemIcon_SubQuick(_uint iSlotIdx);
	HRESULT			Create_ItemIcon_Quick(CItemData* pItemData, _int iInvenIdx);
	HRESULT			Create_ItemIcon_Weapon();
	HRESULT			Create_ItemIcon_Skill();

	HRESULT			Change_ItemIcon_Weapon();
	HRESULT			Change_ItemIcon_Skill();

	HRESULT			Pull_ItemIcon(_bool isEquip, wstring wstrTexture, wstring wstrItemName, wstring wstrItemExplain, wstring wstrItemExplain_Quick); // 빈 Slot에 다음 슬롯의 ItemIcon을 당겨오는 함수
	HRESULT			Delete_ItemIcon();

	void			Resset_Data(); // 이거랑 Delete랑 똑같음...
	void			Clear_ItemIcon(); // nullptr로만 만들어주는 함수
	void			Check_Equip(_bool isWeapon, CItemData* pItemData);

	HRESULT			Create_RedDot();
	HRESULT			Delete_RedDot();

private:
	_bool						m_isEquip = { false };

	_int						m_iInventoryIdx = { -1 }; // Quick Slot의 경우 Inventory의 어느 Item을 가지는지
	_uint						m_iSlotIdx = { 0 };
	wstring						m_wstrTexture = TEXT("");
	wstring						m_wszItemName = TEXT("");
	wstring						m_wszItemExplain = TEXT("");
	wstring						m_wszItemExplain_Quick = TEXT("");

	UISORT_PRIORITY				m_eUISort = { SORT_END };
	SLOT_TYPE					m_eSlotType = { SLOT_END };

	CUI_Slot_Frame*				m_pSelectFrame = { nullptr };
	CUI_ItemIcon*				m_pItemIcon = { nullptr };
	CUI_ItemIcon*				m_pSymbolIcon = { nullptr };
	CUI_Slot_EquipSign*			m_pEquipSign = { nullptr };

	class CUI_RedDot*			m_pRedDot = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	HRESULT	Create_Frame();
	HRESULT	Create_EquipSign();

	void	Click_BtnEvent();
	void	Render_Font();
	void	Rend_Count();
	_bool	Check_GroupRenderOnAnim();

public:
	static CUI_Slot*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END