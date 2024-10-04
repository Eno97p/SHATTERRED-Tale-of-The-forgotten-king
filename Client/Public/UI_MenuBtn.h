#pragma once

#include "UI_Interaction.h"

BEGIN(Client)

class CUI_MenuBtn final : public CUI_Interaction
{
public:
	enum MENU_TYPE { MENU_CH, MENU_MAP, MENU_WEAPON, MENU_INV, MENU_SET, MENU_COD, MENU_END };

	typedef struct UI_MenuBtn_Desc : public UI_DESC
	{
		MENU_TYPE		eMenuType;
	}UI_MENUBTN_DESC;

private:
	CUI_MenuBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MenuBtn(const CUI_MenuBtn& rhs);
	virtual ~CUI_MenuBtn() = default;

public:
	MENU_TYPE		Get_MenuType() { return m_eMenuType; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	HRESULT	Create_RedDot();
	HRESULT Delete_RedDot();

	void	Set_RedDot_Rend(_bool isRend);

private:
	_uint				m_iTextureNum = { 0 };

	MENU_TYPE			m_eMenuType = { MENU_END };
	
	class CUI_RedDot*	m_pRedDot = { nullptr };
	
private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	Setting_XY();
	wstring	Setting_Texture();
	_tchar* Settiing_BtnText();

	void	Open_MenuPage();

public:
	static CUI_MenuBtn*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END