#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_QTE_Particle final : public CUI
{
private:
	CUI_QTE_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_QTE_Particle(const CUI_QTE_Particle& rhs);
	virtual ~CUI_QTE_Particle() = default;

public:
	_bool			Get_isDead() { return m_isDead; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool			m_isDead = { false };
	_float			m_fDeadTimer = { 0.f };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_QTE_Particle*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END