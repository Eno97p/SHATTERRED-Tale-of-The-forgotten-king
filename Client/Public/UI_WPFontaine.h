#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_WPFontaine final : public CUI
{
public:
	typedef struct UI_Fontaine_Desc : public UI_DESC
	{
		_uint		iSlotNum;
	}UI_FONTAINE_DESC;

private:
	CUI_WPFontaine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_WPFontaine(const CUI_WPFontaine& rhs);
	virtual ~CUI_WPFontaine() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_uint			m_iSlotNum = { 0 };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_WPFontaine*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END