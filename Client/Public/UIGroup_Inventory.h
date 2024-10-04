#pragma once

#include "UIGroup.h"

BEGIN(Client)
class CUI;
class CUI_Slot;

class CUIGroup_Inventory final : public CUIGroup
{
private:
	CUIGroup_Inventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Inventory(const CUIGroup_Inventory& rhs);
	virtual ~CUIGroup_Inventory() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void			Update_Inventory_Add(_uint iSlotIdx);
	void			Update_Slot_EquipSign(_uint iCurSlotIdx, _bool isEquip);
	void			Update_Inventory_Delete(_uint iSlotIdx);

	void			Rend_Calcul(_int iSoul);
	void			Update_Inventory(_uint iSlotIdx);

	HRESULT			Create_RedDot(_uint iSlotIdx);
	HRESULT			Delete_RedDot();

private:
	vector<CUI*>			m_vecUI;
	vector<CUI_Slot*>		m_vecSlot;
	class CUI_StateSoul*	m_pSoul = { nullptr };

private:
	HRESULT					Create_UI();
	HRESULT					Create_Slot();

public:
	static CUIGroup_Inventory*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END