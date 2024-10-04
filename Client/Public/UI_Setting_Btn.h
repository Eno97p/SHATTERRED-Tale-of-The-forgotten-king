#pragma once

#include "UI_Interaction.h"

BEGIN(Client)

class CUI_Setting_Btn final : public CUI_Interaction
{
public:
	enum BTN_TYPE { BTN_TEST1 = 0, BTN_TEST2, BTN_TEST3, BTN_TEST4, BTN_END };
	typedef struct UI_SettingBtn_Desc : public UI_DESC
	{ 
		_uint		iBtnIdx;
	}UI_SETTINGBTN_DESC;

private:
	CUI_Setting_Btn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Setting_Btn(const CUI_Setting_Btn& rhs);
	virtual ~CUI_Setting_Btn() = default;

public:
	_bool			Get_isOn() { return m_isOn; }
	void			Set_isOn(_bool isOn) { m_isOn = isOn; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool			m_isOn = { false };
	wstring			wstrBtnText;

	BTN_TYPE		m_eBtnType = { BTN_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	Rend_Font();
	void	Setting_BtnData(_uint iBtnIdx);

public:
	static CUI_Setting_Btn*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END