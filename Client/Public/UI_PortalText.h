#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_PortalText final : public CUI
{
public:
	typedef struct UI_PortalText_Desc : public UI_DESC
	{
		_float	fDistance;
		_vector vPos;
		LEVEL	ePortalLevel;
	}UI_PORTALTEXT_DESC;

private:
	CUI_PortalText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PortalText(const CUI_PortalText& rhs);
	virtual ~CUI_PortalText() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool			m_isRend = { false };
	_float			m_fFontX = { 0.f };
	_float			m_fDistance = { 0.f };
	_vector			m_vTargetPos;
	wstring			m_fText;

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	Setting_Text(LEVEL ePortalLevel);
	void	Check_Distance();

public:
	static CUI_PortalText*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END