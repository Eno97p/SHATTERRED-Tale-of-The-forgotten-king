#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Dash final : public CUI
{
public:
	typedef struct UI_Dash_Desc : public UI_DESC
	{
		_uint iDashIdx;
	}UI_DASH_DESC;
private:
	CUI_Dash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Dash(const CUI_Dash& rhs);
	virtual ~CUI_Dash() = default;

public:
	_bool			Get_Rend() { return m_isRend; }
	void			Set_Rend(_bool isRend) { m_isRend = isRend; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_uint			m_iDashIdx = { 0 };

private:
	_bool			m_isRend = { false };
	_bool			m_isAnimUp = { false };
	_float			m_fAlphaTimer = { 0.f };
	_float			m_fAnimTimer = { 0.f };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_Dash*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END