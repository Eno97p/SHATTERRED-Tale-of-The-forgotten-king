#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_BossShield final : public CUI
{
private:
	CUI_BossShield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BossShield(const CUI_BossShield& rhs);
	virtual ~CUI_BossShield() = default;

public:
	void			Set_Ratio(_float fRatio) { m_fCurrentRatio = fRatio; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool				m_isDamageRend = { false };
	_int				m_iAccumDamage = { 0 };
	_float				m_fCurrentRatio = 1.f;
	_float				m_fPastRatio = 1.f;
	_float				m_fHudRatio = 0.55f;
	_float				m_fDamageTimer = { 0.f };

	class CMonster* m_pMonster = { nullptr }; // Boss·Î º¯°æ

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_BossShield*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END