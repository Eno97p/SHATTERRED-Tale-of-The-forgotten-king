#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Slot_Frame final : public CUI
{
public:
	typedef struct UI_SlotFrame_Desc : public UI_DESC
	{
		UISORT_PRIORITY		eUISort;
	}UI_SLOTFRAME_DESC;
private:
	CUI_Slot_Frame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Slot_Frame(const CUI_Slot_Frame& rhs);
	virtual ~CUI_Slot_Frame() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	UISORT_PRIORITY						m_eUISort = { SORT_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_Slot_Frame*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END