#pragma once

#include "UI_Interaction.h"

BEGIN(Client)

class CUI_LogoSelector final : public CUI_Interaction
{
public:
	enum SELECTOR_TYPE { SELECTOR_CONTINUE, SELECTOR_NEWGAME, SELECTOR_SETTINGS, SELECTOR_CREDITS, SELECTOR_LEAVE, SELECTOR_END };
	typedef struct UI_Selector_Desc : public UI_DESC
	{
		SELECTOR_TYPE	eSelectorType;
	}UI_SELECTOR_DESC;

private:
	CUI_LogoSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_LogoSelector(const CUI_LogoSelector& rhs);
	virtual ~CUI_LogoSelector() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	SELECTOR_TYPE		m_eSelectorType = { SELECTOR_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	_tchar*	Setting_SelectorText();
	HRESULT Click_Event();

public:
	static CUI_LogoSelector*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END