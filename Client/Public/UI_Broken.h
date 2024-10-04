#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Broken final : public CUI
{
private:
	CUI_Broken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Broken(const CUI_Broken& rhs);
	virtual ~CUI_Broken() = default;

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
	_bool			m_isRend = { false };
	_float			m_fDeadTimer = { 0.f };
	_float			m_fAlphaTimer = { 0.f };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_Broken*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END