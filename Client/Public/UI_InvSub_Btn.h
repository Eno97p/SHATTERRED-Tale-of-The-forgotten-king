#pragma once

#include "UI_Interaction.h"

BEGIN(Client)
class CUI_InvSub_BtnSelect; 

class CUI_InvSub_Btn final : public CUI_Interaction
{
public:
	enum BTN_TYPE { BTN_SET = 0, BTN_USE, BTN_CANCEL, BTN_END };
	typedef struct UI_Btn_Desc : public UI_DESC
	{
		_uint		iSlotIdx;
		BTN_TYPE	eBtnType;
	}UI_BTN_DESC;

private:
	CUI_InvSub_Btn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_InvSub_Btn(const CUI_InvSub_Btn& rhs);
	virtual ~CUI_InvSub_Btn() = default;

public:
	void			Set_SlotIdx(_uint iSlotIdx) { m_iSlotIdx = iSlotIdx; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool					m_isSelectEnd = { false };
	_uint					m_iSlotIdx = { 0 };
	_float					m_fSelectedTimer = { 0.f };

	BTN_TYPE				m_eBtnType = { BTN_END };

	CUI_InvSub_BtnSelect*	m_pSelectBtn = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	HRESULT	Create_SelectBtn();

	_tchar* Settiing_BtnText();

	void	Mouse_Input();

public:
	static CUI_InvSub_Btn*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END