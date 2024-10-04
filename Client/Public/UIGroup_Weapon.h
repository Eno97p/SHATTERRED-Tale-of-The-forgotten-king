#pragma once

#include "UIGroup.h"

BEGIN(Client)
class CUI;
class CUI_Slot;

class CUIGroup_Weapon final : public CUIGroup
{
public:
	enum TAB_TYPE { TAB_L, TAB_R, TAB_END };

private:
	CUIGroup_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Weapon(const CUIGroup_Weapon& rhs);
	virtual ~CUIGroup_Weapon() = default;

public:
	TAB_TYPE		Get_TabType() { return m_eTabType; }

	_bool			Get_EquipMode() { return m_isEquipMode; }
	void			Set_EquipMode(_bool isEquipMode) { m_isEquipMode = isEquipMode; }

	_uint			Get_CurSlotIdx() { return m_iCurSlotIdx; }
	void			Set_CurSlotIdx(_uint iCurSlotIdx) { m_iCurSlotIdx = iCurSlotIdx; }
	
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void			Update_Weapon_Add();
	void			Update_EquipSlot_Add(_uint iEquipSlotIdx);
	void			Update_EquipSlot_Delete(_uint iEquipSlotIdx);
	void			Update_Skill_Add();
	
	void			Update_Slot_EquipSign(_bool isEquip, _uint iSlotIdx = 0);
	
	void			Reset_Tab();
	
	HRESULT			Create_RedDot(_uint iSlotIdx, _bool isSkill = false);
	HRESULT			Delete_RedDot();

	_bool			Check_RedDot();

private:
	_bool						m_isEquipMode = { false }; // 장착 활성화 여부
	_uint						m_iCurSlotIdx = { 0 }; // 현재 선택한 Weapon Slot의 Idx

	vector<CUI*>					m_vecUI;
	vector<CUI_Slot*>				m_vecSlot;
	vector<class CUI_WPEquipSlot*>	m_vecEquipSlot;
	class CUI_MenuPage_BGAlpha*		m_pAlphaBG = { nullptr };

	TAB_TYPE				m_eTabType = { TAB_END };

private:
	HRESULT					Create_UI();
	HRESULT					Create_Slot();
	HRESULT					Create_EquipSlot();
	HRESULT					Create_Fontaine();
	HRESULT					Create_Tab();

	void					Change_Tab();

public:
	static CUIGroup_Weapon*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END