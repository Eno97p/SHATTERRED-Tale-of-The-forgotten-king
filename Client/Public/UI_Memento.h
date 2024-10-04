#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Memento final : public CUI
{
private:
	CUI_Memento(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Memento(const CUI_Memento& rhs);
	virtual ~CUI_Memento() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool			m_isRend = { true };
	_bool			m_isRendOn = { true };
	_float			m_fColor = { 0.f };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_Memento*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END