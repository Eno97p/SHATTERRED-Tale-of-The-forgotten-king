#pragma once

#include "UIGroup.h"

BEGIN(Client)
class CUI;
class CUI_Slot;
class CUI_InvSub_Btn;

class CUIGroup_InvSub final : public CUIGroup
{
private:
	CUIGroup_InvSub(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_InvSub(const CUIGroup_InvSub& rhs);
	virtual ~CUIGroup_InvSub() = default;

public:
	void			Set_SlotIdx(_uint iSlotIdx) { m_iSlotIdx = iSlotIdx; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void			Update_InvSub_QuickSlot(_uint iSlotIdx);
	void			Delete_InvSub_QuickSlot(_uint iSlotIdx);
	void			Delete_InvSub_QuickSlot_ToInvIdx(_uint iInvIdx);

private:
	_uint					m_iSlotIdx = { 0 }; // 이 SubPage가 현재 Inventory의 몇 번째 Slot에 있는 Item에 대한 정보를 가지고 있는지
	
	vector<CUI*>			m_vecUI;
	vector<CUI_InvSub_Btn*>	m_vecBtn;
	vector<CUI_Slot*>		m_vecSlot;

private:
	HRESULT					Create_UI();
	HRESULT					Create_Slot();
	HRESULT					Create_Btn();

public:
	static CUIGroup_InvSub*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END