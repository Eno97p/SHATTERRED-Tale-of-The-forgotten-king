#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Menu_SelectFrame final : public CUI
{
public:
	enum MENU_TYPE { MENU_CH, MENU_MAP, MENU_WEAPON, MENU_INV, MENU_SET, MENU_COD, MENU_END };

	typedef struct UI_MenuSelectFrame_Desc : public UI_DESC
	{
		MENU_TYPE		eMenuType;
	}UI_MENUSELECTFRAME_DESC;

private:
	CUI_Menu_SelectFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Menu_SelectFrame(const CUI_Menu_SelectFrame& rhs);
	virtual ~CUI_Menu_SelectFrame() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_float			m_fFlowTimer = {0.f};

	MENU_TYPE		m_eMenuType = { MENU_END };

	CTexture*		m_pMaskTextureCom = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	Setting_XY();

public:
	static CUI_Menu_SelectFrame*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END