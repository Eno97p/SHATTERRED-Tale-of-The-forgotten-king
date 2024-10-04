#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)
class CUI;
class CUI_Slot;
class CItemData;

class CUIGroup_Quick final : public CUIGroup
{
private:
	CUIGroup_Quick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Quick(const CUIGroup_Quick& rhs);
	virtual ~CUIGroup_Quick() = default;

public:
	_bool			Get_isInvSlot_Act() { return m_isInvSlot_Act; }
	void			Set_isInvSlot_Act(_bool isInvSlot_Act) { m_isInvSlot_Act = isInvSlot_Act; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void			Update_QuickSlot_Add(CItemData* pItemData, _int iInvenIdx);
	void			Update_InvSlot_Add(_uint iSlotIdx);
	void			Update_InvSlot_EquipSign(_uint iSlotIdx, _bool isEquip);

	void			Update_QuickSlot_Delete(_uint iInvenIdx);
	void			Update_InvSlot_Delete(_uint iSlotIdx);

	void			Update_Inventory(_uint iSlotIdx);

	HRESULT			Create_RedDot(_uint iSlotIdx);
	HRESULT			Delete_RedDot();

private:
	_bool					m_isInvSlot_Act = { false }; // InvSlot 활성화 여부

	vector<CUI*>			m_vecUI;
	vector<CUI_Slot*>		m_vecSlot;
	CUI*					m_pInvSlotBG = { nullptr };
	vector<CUI_Slot*>		m_vecInvSlot;

private:
	HRESULT					Create_UI();
	HRESULT					Create_Slot();
	HRESULT					Create_InvSlot();

public:
	static CUIGroup_Quick*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END