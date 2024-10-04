#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_BossHP final : public CUI
{
private:
	CUI_BossHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BossHP(const CUI_BossHP& rhs);
	virtual ~CUI_BossHP() = default;

public:
	void			Set_Ratio(_float fRatio) { m_fCurrentRatio = fRatio; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	void			Rend_Damage(_int iValue);

private:
	_bool				m_isDamageRend = { false };
	_int				m_iAccumDamage = { 0 };
	_float				m_fCurrentRatio = 1.f;
	_float				m_fPastRatio = 1.f;
	_float				m_fHudRatio = 0.55f;
	_float				m_fDamageTimer = { 0.f };
	wstring				m_wstrDamage;

	class CMonster*		m_pMonster = { nullptr }; // Boss·Î º¯°æ

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_BossHP*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END