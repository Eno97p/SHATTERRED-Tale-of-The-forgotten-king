#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_WeaponTab final : public CUI
{
	const _float SELECT_SIZE = 32.f;
	const _float NONE_SIZE = 26.f;
public:
	enum TAB_TYPE { TAB_L, TAB_R, TAB_END };
	typedef struct UI_Tab_Desc : public UI_DESC
	{
		TAB_TYPE	eTabType;
	}UI_TAB_DESC;

private:
	CUI_WeaponTab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_WeaponTab(const CUI_WeaponTab& rhs);
	virtual ~CUI_WeaponTab() = default;

public:
	_bool			Get_isActivate() { return m_isActivate; }
	void			Set_isActivate(_bool isActivate) { m_isActivate = isActivate; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool			m_isActivate = { false }; // 활성화 여부

	_float			m_fFontX = { 0.f };

	TAB_TYPE		m_eTabType = { TAB_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	Setting_Data();
	_tchar* Settiing_BtnText();

	void	Render_Text();
	void	Change_Activate();
	void	Change_Size();

public:
	static CUI_WeaponTab*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END